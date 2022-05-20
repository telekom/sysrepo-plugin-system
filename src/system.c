#include <system.h>
#include <common.h>
#include <context.h>
#include <utils/memory.h>

// stdlib
#include <stdbool.h>

// sysrepo
#include <sysrepo.h>

// libyang
#include <libyang/tree_data.h>

// bridging
#include "startup.h"
#include "subscription/change.h"
#include "subscription/operational.h"
#include "subscription/rpc.h"

// check if the datastore which the session uses is empty (startup or running)
static bool system_check_empty_datastore(sr_session_ctx_t *session);

int sr_plugin_init_cb(sr_session_ctx_t *running_session, void **private_data)
{
	int error = 0;

	// sysrepo
	sr_session_ctx_t *startup_session = NULL;
	sr_conn_ctx_t *connection = NULL;
	sr_subscription_ctx_t *subscription = NULL;

	// plugin
	system_ctx_t *ctx = NULL;

	// init context
	ctx = xmalloc(sizeof(*ctx));
	*ctx = (system_ctx_t){0};

	*private_data = ctx;

	// module changes
	struct system_module_change module_changes[] = {
		{
			SYSTEM_CONTACT_YANG_PATH,
			system_change_contact,
		},
		{
			SYSTEM_HOSTNAME_YANG_PATH,
			system_change_hostname,
		},
		{
			SYSTEM_LOCATION_YANG_PATH,
			system_change_location,
		},
		{
			SYSTEM_TIMEZONE_NAME_YANG_PATH,
			system_change_timezone_name,
		},
		{
			SYSTEM_TIMEZONE_UTC_OFFSET_YANG_PATH,
			system_change_timezone_utc_offset,
		},
		{
			SYSTEM_NTP_ENABLED_YANG_PATH,
			system_change_ntp_enabled,
		},
		{
			SYSTEM_NTP_SERVER_YANG_PATH,
			system_change_ntp_server,
		},
		{
			SYSTEM_DNS_RESOLVER_SEARCH_YANG_PATH,
			system_change_dns_resolver_search,
		},
		{
			SYSTEM_DNS_RESOLVER_SERVER_YANG_PATH "//*",
			system_change_dns_resolver_server,
		},
		{
			SYSTEM_DNS_RESOLVER_TIMEOUT_YANG_PATH,
			system_change_dns_resolver_timeout,
		},
		{
			SYSTEM_DNS_RESOLVER_ATTEMPTS_YANG_PATH,
			system_change_dns_resolver_attempts,
		},
		{
			SYSTEM_AUTHENTICATION_USER_AUTHENTICATION_ORDER_YANG_PATH,
			system_change_authentication_user_authentication_order,
		},
		{
			SYSTEM_AUTHENTICATION_USER_YANG_PATH,
			system_change_authentication_user,
		},
	};

	// rpcs
	struct system_rpc rpcs[] = {
		{
			SYSTEM_SET_CURRENT_DATETIME_RPC_YANG_PATH,
			system_rpc_set_current_datetime,
		},
		{
			SYSTEM_RESTART_RPC_YANG_PATH,
			system_rpc_restart,
		},

		{
			SYSTEM_SHUTDOWN_RPC_YANG_PATH,
			system_rpc_shutdown,
		},
	};

	// operational getters
	struct system_operational oper[] = {
		{
			SYSTEM_STATE_PLATFORM_YANG_PATH "/*",
			system_operational_platform,
		},
		{
			SYSTEM_STATE_CLOCK_YANG_PATH "/*",
			system_operational_clock,
		},
	};

	connection = sr_session_get_connection(running_session);
	error = sr_session_start(connection, SR_DS_STARTUP, &startup_session);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_session_start() error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	ctx->startup_session = startup_session;

	if (system_check_empty_datastore(startup_session)) {
		SRPLG_LOG_INF(PLUGIN_NAME, "Startup datasore is empty");
		SRPLG_LOG_INF(PLUGIN_NAME, "Loading initial system data");
		error = system_startup_load_data(ctx, startup_session);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "Error loading initial data into the startup datastore... exiting");
			goto error_out;
		}

		// copy contents of the startup session to the current running session
		error = sr_copy_config(running_session, BASE_YANG_MODEL, SR_DS_STARTUP, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config() error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	// subscribe every module change
	for (size_t i = 0; i < ARRAY_SIZE(module_changes); i++) {
		const struct system_module_change *change = &module_changes[i];

		// in case of work on a specific callback set it to NULL
		if (change->cb) {
			error = sr_module_change_subscribe(running_session, BASE_YANG_MODEL, change->path, change->cb, *private_data, 0, SR_SUBSCR_DEFAULT, &subscription);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "sr_module_change_subscribe() error for \"%s\" (%d): %s", change->path, error, sr_strerror(error));
				goto error_out;
			}
		}
	}

	// subscribe every rpc
	for (size_t i = 0; i < ARRAY_SIZE(rpcs); i++) {
		const struct system_rpc *rpc = &rpcs[i];

		// in case of work on a specific callback set it to NULL
		if (rpc->cb) {
			error = sr_rpc_subscribe(running_session, rpc->path, rpc->cb, *private_data, 0, SR_SUBSCR_DEFAULT, &subscription);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "sr_rpc_subscribe error (%d): %s", error, sr_strerror(error));
				goto error_out;
			}
		}
	}

	for (size_t i = 0; i < ARRAY_SIZE(oper); i++) {
		const struct system_operational *op = &oper[i];

		// in case of work on a specific callback set it to NULL
		if (op->cb) {
			error = sr_oper_get_subscribe(running_session, BASE_YANG_MODEL, op->path, op->cb, NULL, SR_SUBSCR_DEFAULT, &subscription);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "sr_oper_get_subscribe() error (%d): %s", error, sr_strerror(error));
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
	int error = 0;

	system_ctx_t *ctx = (system_ctx_t *) private_data;

	// save current running configuration into startup for next time when the plugin starts
	error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config() error (%d): %s", error, sr_strerror(error));
	}

	FREE_SAFE(ctx);
}

static bool system_check_empty_datastore(sr_session_ctx_t *session)
{
	int error = SR_ERR_OK;
	bool is_empty = true;
	sr_val_t *values = NULL;
	size_t value_cnt = 0;

	error = sr_get_items(session, SYSTEM_HOSTNAME_YANG_PATH, 0, SR_OPER_DEFAULT, &values, &value_cnt);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_items() error (%d): %s", error, sr_strerror(error));
		goto out;
	}

	if (value_cnt > 0) {
		sr_free_values(values, value_cnt);
		is_empty = false;
	}

out:
	return is_empty;
}