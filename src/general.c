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

#include "general.h"
#include "context.h"
#include "common.h"
#include "initial_load.h"
#include "callbacks.h"
#include "utils/memory.h"
#include "utils/ntp/server_list.h"
#include "utils/dns/search.h"
#include "utils/dns/server.h"
#include "utils/user_auth/user_authentication.h"

// uthash library
#include "utlist.h"
#include "utarray.h"

#include <sysrepo.h>
#include <sysrepo/xpath.h>

static bool system_running_datastore_is_empty_check(void);

int sr_plugin_init_cb(sr_session_ctx_t *session, void **private_data)
{
	int error = 0;
	sr_conn_ctx_t *connection = NULL;
	sr_session_ctx_t *startup_session = NULL;
	sr_subscription_ctx_t *subscription = NULL;
	char *location_file_path = NULL;
	char *ntp_names_file_path = NULL;
	*private_data = NULL;

	// create plugin context
	system_ctx_t *ctx = (system_ctx_t *) xmalloc(sizeof(system_ctx_t));

	// set memory to zero
	*ctx = (system_ctx_t){0};
	*private_data = ctx;

	location_file_path = system_get_plugin_file_path(LOCATION_FILENAME, true);
	if (location_file_path == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Please set the %s env variable. "
								   "The plugin uses the path in the variable "
								   "to store location in a file.",
					  PLUGIN_DIR_ENV_VAR);
		error = -1;
		goto error_out;
	}

	ntp_names_file_path = system_get_plugin_file_path(NTP_NAMES_FILENAME, true);
	if (ntp_names_file_path == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Please set the %s env variable. "
								   "The plugin uses the path in the variable "
								   "to store ntp server names in a file.",
					  PLUGIN_DIR_ENV_VAR);
		error = -1;
		goto error_out;
	}

	error = local_user_array_init(&ctx->local_users);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "local_users_array_init() failed: %d", error);
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "start session to startup datastore");

	connection = sr_session_get_connection(session);
	error = sr_session_start(connection, SR_DS_STARTUP, &startup_session);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_session_start error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	// setup private data to carry the plugin context throughout the plugin execution
	ctx->startup_session = startup_session;
	*private_data = ctx;

	if (system_running_datastore_is_empty_check() == true) {
		SRPLG_LOG_INF(PLUGIN_NAME, "running DS is empty, loading data");
		error = system_initial_load(ctx, session);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "load_data error");
			goto error_out;
		}

		error = sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	// ntp servers array
	error = ntp_server_array_init(session, &ctx->ntp_servers);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "ntp_server_array_init() error: %d", error);
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "subscribing to module change");

	error = sr_module_change_subscribe(session, BASE_YANG_MODEL, "/" BASE_YANG_MODEL ":*", system_module_change_cb, *private_data, 0, 0, &subscription);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_module_change_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "subscribing to get oper items");

	error = sr_oper_get_subscribe(session, BASE_YANG_MODEL, SYSTEM_STATE_YANG_MODEL, system_state_data_cb, NULL, 1, &subscription);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_oper_get_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "subscribing to rpc");

	error = sr_rpc_subscribe(session, SET_CURR_DATETIME_YANG_PATH, system_rpc_cb, *private_data, 0, 1, &subscription);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_rpc_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	error = sr_rpc_subscribe(session, RESTART_YANG_PATH, system_rpc_cb, *private_data, 0, 1, &subscription);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_rpc_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	error = sr_rpc_subscribe(session, SHUTDOWN_YANG_PATH, system_rpc_cb, *private_data, 0, 1, &subscription);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_rpc_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "plugin init done");

	FREE_SAFE(location_file_path);
	FREE_SAFE(ntp_names_file_path);

	goto out;

error_out:
	if (subscription != NULL) {
		sr_unsubscribe(subscription);
	}

	if (location_file_path != NULL) {
		FREE_SAFE(location_file_path);
	}

	if (ntp_names_file_path != NULL) {
		FREE_SAFE(ntp_names_file_path);
	}

out:
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

void sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *private_data)
{
	dns_server_element_t *iter = NULL, *tmp = NULL;
	system_ctx_t *ctx = (system_ctx_t *) private_data;

	sr_session_ctx_t *startup_session = ctx->startup_session;

	if (startup_session) {
		sr_session_stop(startup_session);
	}

	if (ctx->ntp_servers) {
		ntp_server_array_free(&ctx->ntp_servers);
	}

	if (ctx->dns_servers_head) {
		LL_FOREACH_SAFE(ctx->dns_servers_head, iter, tmp)
		{
			// free data structure
			dns_server_free(&iter->server);

			// free list element
			LL_DELETE(ctx->dns_servers_head, iter);
			free(iter);
		}
	}

	if (ctx->local_users) {
		local_user_array_free(&ctx->local_users);
	}

	// free context memory at the end after cleaning up all context members
	FREE_SAFE(ctx);

	SRPLG_LOG_INF(PLUGIN_NAME, "plugin cleanup finished");
}

static bool system_running_datastore_is_empty_check(void)
{
	FILE *sysrepocfg_DS_empty_check = NULL;
	bool is_empty = false;

	sysrepocfg_DS_empty_check = popen(SYSREPOCFG_EMPTY_CHECK_COMMAND, "r");
	if (sysrepocfg_DS_empty_check == NULL) {
		SRPLG_LOG_WRN(PLUGIN_NAME, "could not execute %s", SYSREPOCFG_EMPTY_CHECK_COMMAND);
		is_empty = true;
		goto out;
	}

	if (fgetc(sysrepocfg_DS_empty_check) == EOF) {
		is_empty = true;
	}

out:
	if (sysrepocfg_DS_empty_check) {
		pclose(sysrepocfg_DS_empty_check);
	}

	return is_empty;
}