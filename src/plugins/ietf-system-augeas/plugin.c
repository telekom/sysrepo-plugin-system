/*
 * telekom / sysrepo-plugin-system
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2022 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "plugin.h"
#include "core/common.h"
#include "core/context.h"

// stdlib
#include <stdbool.h>

// sysrepo
#include <sysrepo.h>

// libyang
#include <libyang/tree_data.h>

#include "srpc/common.h"
#include "srpc/feature_status.h"
#include "srpc/types.h"

// startup
#include "datastore/running/load.h"
#include "datastore/running/store.h"

// subs
#include "core/subscription/change.h"
#include "core/subscription/operational.h"
#include "core/subscription/rpc.h"

#include <srpc.h>

int sr_plugin_init_cb(sr_session_ctx_t *running_session, void **private_data)
{
	int error = 0;

	bool empty_startup = false;

	// sysrepo
	sr_session_ctx_t *startup_session = NULL;
	sr_conn_ctx_t *connection = NULL;
	sr_subscription_ctx_t *subscription = NULL;

	// plugin
	system_ctx_t *ctx = NULL;

	// init context
	ctx = malloc(sizeof(*ctx));
	*ctx = (system_ctx_t){0};

	*private_data = ctx;

	// module changes
	srpc_module_change_t module_changes[] = {
		{
			SYSTEM_HOSTNAME_YANG_PATH,
			system_subscription_change_hostname,
		},
		{
			SYSTEM_NTP_ENABLED_YANG_PATH,
			system_subscription_change_ntp_enabled,
		},
		{
			SYSTEM_NTP_SERVER_YANG_PATH,
			system_subscription_change_ntp_server,
		},
	};

	ctx->ietf_system_features = srpc_feature_status_hash_new();

	// load feature status
	SRPC_SAFE_CALL_ERR(error, srpc_feature_status_hash_load(&ctx->ietf_system_features, running_session, "ietf-system"), error_out);

	// log status of features
	const char *features[] = {
		"radius",
		"authentication",
		"local-users",
		"radius-authentication",
		"ntp",
		"ntp-udp-port",
		"timezone-name",
		"dns-udp-tcp-port",
	};

	SRPLG_LOG_INF(PLUGIN_NAME, "Checking ietf-system YANG module used features");

	for (size_t i = 0; i < ARRAY_SIZE(features); i++) {
		const char *feature = features[i];

		SRPLG_LOG_INF(PLUGIN_NAME, "ietf-system feature \"%s\" status = %s", feature, srpc_feature_status_hash_check(ctx->ietf_system_features, feature) ? "enabled" : "disabled");
	}

	connection = sr_session_get_connection(running_session);
	error = sr_session_start(connection, SR_DS_STARTUP, &startup_session);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_session_start() error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	ctx->startup_session = startup_session;

	error = srpc_check_empty_datastore(running_session, SYSTEM_HOSTNAME_YANG_PATH, &empty_startup);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Failed checking datastore contents: %d", error);
		goto error_out;
	}

	if (empty_startup) {
		SRPLG_LOG_INF(PLUGIN_NAME, "Running datastore is empty");
		SRPLG_LOG_INF(PLUGIN_NAME, "Loading initial system data");

		// load data only into running DS - do not use startup unless said explicitly
		error = system_aug_running_ds_load(ctx, running_session);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "Error loading initial data into the running datastore... exiting");
			goto error_out;
		}
	} else {
		// make sure the data from startup DS is stored in the system
		SRPLG_LOG_INF(PLUGIN_NAME, "Running datastore contains data");
		SRPLG_LOG_INF(PLUGIN_NAME, "Storing running datastore data in the system");

		// check and apply if needed data from startup to the system
		error = system_aug_running_ds_store(ctx, startup_session);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "Error applying initial data from startup datastore to the system... exiting");
			goto error_out;
		}
	}

	// subscribe every module change
	for (size_t i = 0; i < ARRAY_SIZE(module_changes); i++) {
		const srpc_module_change_t *change = &module_changes[i];

		// in case of work on a specific callback set it to NULL
		if (change->cb) {
			error = sr_module_change_subscribe(running_session, BASE_YANG_MODULE, change->path, change->cb, *private_data, 0, SR_SUBSCR_DEFAULT, &subscription);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "sr_module_change_subscribe() error for \"%s\" (%d): %s", change->path, error, sr_strerror(error));
				goto error_out;
			}
		}
	}

	goto out;

error_out:
	error = -1;
	SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured while initializing the plugin (%d)", error);

out:
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

void sr_plugin_cleanup_cb(sr_session_ctx_t *running_session, void *private_data)
{
	system_ctx_t *ctx = (system_ctx_t *) private_data;

	if (ctx->ietf_system_features) {
		srpc_feature_status_hash_free(&ctx->ietf_system_features);
	}

	free(ctx);
}
