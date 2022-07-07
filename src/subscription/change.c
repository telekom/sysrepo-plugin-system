#include "change.h"
#include "common.h"
#include "context.h"
#include "libyang/printer_data.h"
#include "ly_tree.h"
#include "srpc/common.h"
#include "srpc/ly_tree.h"
#include "sysrepo_types.h"
#include "system/data/authentication/local_user/list.h"
#include "system/data/dns_resolver/search/list.h"
#include "system/data/dns_resolver/server/list.h"
#include "system/data/ntp/server/list.h"
#include "types.h"
#include "umgmt/db.h"
#include "utils/memory.h"

// Load API
#include "system/api/load.h"
#include "system/api/dns_resolver/load.h"
#include "system/api/ntp/load.h"

// Change API
#include "system/api/change.h"
#include "system/api/dns_resolver/change.h"
#include "system/api/ntp/change.h"
#include "system/api/authentication/change.h"

// Store API
#include "system/api/store.h"
#include "system/api/dns_resolver/store.h"
#include "system/api/ntp/store.h"

#include <sysrepo.h>
#include <sysrepo/xpath.h>
#include <assert.h>
#include <errno.h>
#include <pwd.h>
#include <linux/limits.h>

#include <srpc.h>

#include <utlist.h>

int system_subscription_change_contact(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;
	system_ctx_t *ctx = (system_ctx_t *) private_data;

	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Aborting changes for %s", xpath);
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config() error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
		error = srpc_iterate_changes(ctx, session, xpath, system_change_contact);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() error (%d)", error);
			goto error_out;
		}
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	return error;
}

int system_subscription_change_hostname(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;
	system_ctx_t *ctx = (system_ctx_t *) private_data;

	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Aborting changes for %s", xpath);
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config() error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
		error = srpc_iterate_changes(ctx, session, xpath, system_change_hostname);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() error (%d)", error);
			goto error_out;
		}
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	return error;
}

int system_subscription_change_location(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;
	system_ctx_t *ctx = (system_ctx_t *) private_data;

	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Aborting changes for %s", xpath);
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config() error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
		error = srpc_iterate_changes(ctx, session, xpath, system_change_location);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() error (%d)", error);
			goto error_out;
		}
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	return error;
}

int system_subscription_change_timezone_name(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;
	system_ctx_t *ctx = (system_ctx_t *) private_data;

	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "aborting changes for: %s", xpath);
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config() error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
		error = srpc_iterate_changes(ctx, session, xpath, system_change_timezone_name);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() error (%d)", error);
			goto error_out;
		}
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	return error;
}

int system_subscription_change_timezone_utc_offset(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;
	system_ctx_t *ctx = (system_ctx_t *) private_data;

	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "aborting changes for: %s", xpath);
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config() error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unsupported option for now");
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	return SR_ERR_CALLBACK_FAILED;
}

int system_subscription_change_ntp_enabled(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;
	system_ctx_t *ctx = (system_ctx_t *) private_data;
	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "aborting changes for: %s", xpath);
		error = -1;
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
	}

	goto out;
error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	return SR_ERR_CALLBACK_FAILED;
}

int system_subscription_change_ntp_server(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;

	char xpath_buffer[PATH_MAX] = {0};
	system_ctx_t *ctx = (system_ctx_t *) private_data;
	system_ntp_server_element_t *iter = NULL;

	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "aborting changes for: %s", xpath);
		error = -1;
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
		// make sure the last change servers were free'd and set to NULL
		assert(ctx->temp_ntp_servers == NULL);

		// load all system NTP servers
		error = system_ntp_load_server(ctx, &ctx->temp_ntp_servers);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_load_server() error (%d)", error);
			goto error_out;
		}

		SRPLG_LOG_DBG(PLUGIN_NAME, "Servers before changes:");
		LL_FOREACH(ctx->temp_ntp_servers, iter)
		{
			SRPLG_LOG_DBG(PLUGIN_NAME, "\t<%s, %s, %s, %s, %s, %s>", iter->server.name, iter->server.address, iter->server.port, iter->server.association_type, iter->server.iburst, iter->server.prefer);
		}

		// process changes and use store API to store the configured list

		// name change
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//name", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
			goto error_out;
		}
		error = srpc_iterate_changes(ctx, session, xpath_buffer, system_ntp_change_server_name);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for name failed: %d", error);
			goto error_out;
		}

		// address change
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//udp/address", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
			goto error_out;
		}
		error = srpc_iterate_changes(ctx, session, xpath_buffer, system_ntp_change_server_address);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for address failed: %d", error);
			goto error_out;
		}

		// port change
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//udp/port", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
			goto error_out;
		}
		error = srpc_iterate_changes(ctx, session, xpath_buffer, system_ntp_change_server_port);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for port failed: %d", error);
			goto error_out;
		}

		// association-type
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//association-type", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
			goto error_out;
		}
		error = srpc_iterate_changes(ctx, session, xpath_buffer, system_ntp_change_server_association_type);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for association-type failed: %d", error);
			goto error_out;
		}

		// iburst
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//iburst", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
			goto error_out;
		}
		error = srpc_iterate_changes(ctx, session, xpath_buffer, system_ntp_change_server_iburst);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for iburst failed: %d", error);
			goto error_out;
		}

		// prefer
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//prefer", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
			goto error_out;
		}
		error = srpc_iterate_changes(ctx, session, xpath_buffer, system_ntp_change_server_prefer);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for prefer failed: %d", error);
			goto error_out;
		}

		SRPLG_LOG_DBG(PLUGIN_NAME, "Servers after changes:");
		LL_FOREACH(ctx->temp_ntp_servers, iter)
		{
			SRPLG_LOG_DBG(PLUGIN_NAME, "\t<%s, %s, %s, %s, %s, %s>", iter->server.name, iter->server.address, iter->server.port, iter->server.association_type, iter->server.iburst, iter->server.prefer);
		}

		// delete entries before applying changes - faster than searching for each server and changing libyang tree
		error = sr_delete_item(ctx->startup_session, "/ntp:ntp[config-file=\"/etc/ntp.conf\"]/config-entries", SR_EDIT_DEFAULT);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_delete_item() error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
		error = sr_apply_changes(ctx->startup_session, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_apply_changes() error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		SRPLG_LOG_INF(PLUGIN_NAME, "Deleted /etc/ntp.conf config file data");

		// store generated data
		error = system_ntp_store_server(ctx, ctx->temp_ntp_servers);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_store_server() error (%d)", error);
			goto error_out;
		}
	}

	goto out;
error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:

	system_ntp_server_list_free(&ctx->temp_ntp_servers);

	return SR_ERR_CALLBACK_FAILED;
}

int system_subscription_change_dns_resolver_search(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;
	system_dns_search_element_t *iter = NULL;

	system_ctx_t *ctx = (system_ctx_t *) private_data;
	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "aborting changes for: %s", xpath);
		error = -1;
		goto error_out;
	} else if (event == SR_EV_DONE) {
		SRPLG_LOG_INF(PLUGIN_NAME, "Done processing changes - storing data into startup");
		error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
		// make sure the last change search values were free'd and set to NULL
		assert(ctx->temp_dns_search == NULL);

		// load all system DNS search domains first
		error = system_dns_resolver_load_search(ctx, &ctx->temp_dns_search);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_load_search() error (%d)", error);
			goto error_out;
		}

		SRPLG_LOG_DBG(PLUGIN_NAME, "Search domains before changes:");
		LL_FOREACH(ctx->temp_dns_search, iter)
		{
			SRPLG_LOG_DBG(PLUGIN_NAME, "\t<%s>", iter->search.domain);
		}

		error = srpc_iterate_changes(ctx, session, xpath, system_dns_resolver_change_search);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for name failed: %d", error);
			goto error_out;
		}

		SRPLG_LOG_DBG(PLUGIN_NAME, "Search domains after changes:");
		LL_FOREACH(ctx->temp_dns_search, iter)
		{
			SRPLG_LOG_DBG(PLUGIN_NAME, "\t<%s>", iter->search.domain);
		}

		error = system_dns_resolver_store_search(ctx, ctx->temp_dns_search);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_store_search() error (%d)", error);
			goto error_out;
		}
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:

	system_dns_search_list_free(&ctx->temp_dns_search);

	return error;
}

int system_subscription_change_dns_resolver_server(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;

	char xpath_buffer[PATH_MAX] = {0};
	system_ctx_t *ctx = (system_ctx_t *) private_data;
	system_dns_server_element_t *iter = NULL;

	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "aborting changes for: %s", xpath);
		error = -1;
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
		// make sure the last change servers were free'd and set to NULL
		assert(ctx->temp_dns_servers == NULL);

		// load all system DNS servers first
		error = system_dns_resolver_load_server(ctx, &ctx->temp_dns_servers);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_load_server() error (%d)", error);
			goto error_out;
		}

		SRPLG_LOG_DBG(PLUGIN_NAME, "Servers before changes:");
		LL_FOREACH(ctx->temp_dns_servers, iter)
		{
			SRPLG_LOG_DBG(PLUGIN_NAME, "\t<%s>", iter->server.name);
		}

		// process changes and use store API to store the configured list

		// name change
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//name", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
			goto error_out;
		}
		error = srpc_iterate_changes(ctx, session, xpath_buffer, system_dns_resolver_change_server_name);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for name failed: %d", error);
			goto error_out;
		}

		// address change
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//udp-and-tcp/address", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
			goto error_out;
		}
		error = srpc_iterate_changes(ctx, session, xpath_buffer, system_dns_resolver_change_server_address);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for address failed: %d", error);
			goto error_out;
		}

		// port change
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//udp-and-tcp/port", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
			goto error_out;
		}
		error = srpc_iterate_changes(ctx, session, xpath_buffer, system_dns_resolver_change_server_port);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for port failed: %d", error);
			goto error_out;
		}

		SRPLG_LOG_DBG(PLUGIN_NAME, "Servers after changes:");
		LL_FOREACH(ctx->temp_dns_servers, iter)
		{
			SRPLG_LOG_DBG(PLUGIN_NAME, "\t<%s>", iter->server.name);
		}

		// store generated data
		error = system_dns_resolver_store_server(ctx, ctx->temp_dns_servers);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_store_server() error (%d)", error);
			goto error_out;
		}
	}

	goto out;
error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:

	system_dns_server_list_free(&ctx->temp_dns_servers);

	return error;
}

int system_subscription_change_dns_resolver_timeout(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;
	system_ctx_t *ctx = (system_ctx_t *) private_data;
	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "aborting changes for: %s", xpath);
		error = -1;
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unsupported option for now");
	}

	goto out;
error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	return SR_ERR_CALLBACK_FAILED;
}

int system_subscription_change_dns_resolver_attempts(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;
	system_ctx_t *ctx = (system_ctx_t *) private_data;
	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "aborting changes for: %s", xpath);
		error = -1;
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unsupported option for now");
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	return SR_ERR_CALLBACK_FAILED;
}

int system_subscription_change_authentication_user_authentication_order(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;
	system_ctx_t *ctx = (system_ctx_t *) private_data;
	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "aborting changes for: %s", xpath);
		error = -1;
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
	}

	goto out;
error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	return SR_ERR_CALLBACK_FAILED;
}

int system_subscription_change_authentication_user(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;

	char xpath_buffer[PATH_MAX] = {0};
	system_ctx_t *ctx = (system_ctx_t *) private_data;

	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "aborting changes for: %s", xpath);
		error = -1;
		goto error_out;
	} else if (event == SR_EV_DONE) {
		error = sr_copy_config(ctx->startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	} else if (event == SR_EV_CHANGE) {
		// assert user database is NULL from the last change
		assert(ctx->temp_users.created == NULL);
		assert(ctx->temp_users.modified == NULL);
		assert(ctx->temp_users.deleted == NULL);

		// name change
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//name", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
			goto error_out;
		}
		error = srpc_iterate_changes(ctx, session, xpath_buffer, system_authentication_change_user_name);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for name failed: %d", error);
			goto error_out;
		}

		// address change
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//password", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
			goto error_out;
		}
		error = srpc_iterate_changes(ctx, session, xpath_buffer, system_authentication_change_user_password);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for password failed: %d", error);
			goto error_out;
		}

		// port change
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//authorized-key", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
			goto error_out;
		}
		error = srpc_iterate_changes(ctx, session, xpath_buffer, system_authentication_change_user_authorized_key);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for port failed: %d", error);
			goto error_out;
		}
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	if (ctx->temp_users.created) {
		system_local_user_list_free(&ctx->temp_users.created);
	}
	if (ctx->temp_users.modified) {
		system_local_user_list_free(&ctx->temp_users.modified);
	}
	if (ctx->temp_users.deleted) {
		system_local_user_list_free(&ctx->temp_users.deleted);
	}

	ctx->temp_users.created = ctx->temp_users.modified = ctx->temp_users.deleted = NULL;

	return SR_ERR_CALLBACK_FAILED;
}