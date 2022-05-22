#include "change.h"
#include "common.h"
#include "context.h"

// submodules - helpers
#include "change/dns_resolver.h"
#include "srpc/types.h"
#include "sysrepo_types.h"
#include "utils/memory.h"

#include <sysrepo.h>
#include <sysrepo/xpath.h>
#include <assert.h>
#include <errno.h>
#include <pwd.h>

#include <srpc.h>

// helpers //

static int system_set_timezone_name(const char *value);
static int system_delete_timezone_name(void);
static int system_apply_dns_server_change(system_ctx_t *ctx, sr_session_ctx_t *session, const char *xpath, srpc_change_cb cb);

// dns-resolver/server callbacks:
static int system_change_dns_server_name(void *priv, sr_session_ctx_t *session, const char *prev_value, const struct lyd_node *node, sr_change_oper_t operation);
static int system_change_dns_server_address(void *priv, sr_session_ctx_t *session, const char *prev_value, const struct lyd_node *node, sr_change_oper_t operation);
static int system_change_dns_server_port(void *priv, sr_session_ctx_t *session, const char *prev_value, const struct lyd_node *node, sr_change_oper_t operation);

////

int system_change_contact(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;
	system_ctx_t *ctx = (system_ctx_t *) private_data;

	// sysrepo
	sr_change_iter_t *changes_iterator = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	const char *prev_value = NULL, *prev_list = NULL;
	int prev_default;

	const char *node_name = NULL;
	const char *node_value = NULL;

	// libyang
	const struct lyd_node *node = NULL;

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
		error = sr_get_changes_iter(session, xpath, &changes_iterator);
		if (error != SR_ERR_OK) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_changes_iter() failed (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		while (sr_get_change_tree_next(session, changes_iterator, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
			// fetch node info
			node_name = LYD_NAME(node);
			node_value = lyd_get_value(node);

			// make sure we're reading the right node
			assert(strcmp(node_name, "contact") == 0);

			// SRPLG_LOG_DBG(PLUGIN_NAME, "Node Path: %s", change_path);
			SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Value: %s; Operation: %d", node_name, node_value, operation);

			// don't do anything - keep location stored in the datastore, no need to apply anywhere for now
			// TODO: discuss
		}
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	return error;
}

int system_change_hostname(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;
	system_ctx_t *ctx = (system_ctx_t *) private_data;

	// sysrepo
	sr_change_iter_t *changes_iterator = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	const char *prev_value = NULL, *prev_list = NULL;
	int prev_default;

	const char *node_name = NULL;
	const char *node_value = NULL;

	// libyang
	const struct lyd_node *node = NULL;

#ifdef AUGYANG
	int augeas_changes = 0;
#endif

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
		error = sr_get_changes_iter(session, xpath, &changes_iterator);
		if (error != SR_ERR_OK) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_changes_iter() failed (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		while (sr_get_change_tree_next(session, changes_iterator, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
			// fetch node info
			node_name = LYD_NAME(node);
			node_value = lyd_get_value(node);

			// make sure we're reading the right node
			assert(strcmp(node_name, "hostname") == 0);

			// SRPLG_LOG_DBG(PLUGIN_NAME, "Node Path: %s", change_path);
			SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Value: %s; Operation: %d", node_name, node_value, operation);

			switch (operation) {
				case SR_OP_CREATED:
				case SR_OP_MODIFIED:
					// edit hostname
					error = sethostname(node_value, strnlen(node_value, HOST_NAME_MAX));
					if (error != 0) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "sethostname() error (%d): %s", error, strerror(errno));
						goto error_out;
					}
#ifdef AUGYANG
					SRPLG_LOG_DBG(PLUGIN_NAME, "Setting /etc/hostname value using augeas datastore plugin");
					error = sr_set_item_str(ctx->startup_session, "/hostname:hostname[config-file=\'/etc/hostname\']/hostname", node_value, NULL, 0);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str() error (%d): %s", error, sr_strerror(error));
					} else {
						SRPLG_LOG_DBG(PLUGIN_NAME, "/etc/hostname set");
						augeas_changes = 1;
					}
#endif
					break;
				case SR_OP_DELETED:
					// remove hostname
					error = sethostname("none", strnlen("none", HOST_NAME_MAX));
					if (error != 0) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "sethostname() error: %s", strerror(errno));
					}

#ifdef AUGYANG
					SRPLG_LOG_DBG(PLUGIN_NAME, "Removing /etc/hostname value - setting to \"none\"");
					error = sr_set_item_str(ctx->startup_session, "/hostname:hostname[config-file=\'/etc/hostname\']/hostname", "none", NULL, 0);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str() error (%d): %s", error, sr_strerror(error));
					} else {
						SRPLG_LOG_DBG(PLUGIN_NAME, "/etc/hostname deleted");
						augeas_changes = 1;
					}
#endif
					break;
				case SR_OP_MOVED:
					// N/A
					break;
			}
		}
	}

	if (augeas_changes) {
		SRPLG_LOG_DBG(PLUGIN_NAME, "Applying /etc/hostname changes");
		error = sr_apply_changes(ctx->startup_session, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_apply_changes() error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	return error;
}

int system_change_location(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;
	system_ctx_t *ctx = (system_ctx_t *) private_data;

	// sysrepo
	sr_change_iter_t *changes_iterator = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	const char *prev_value = NULL, *prev_list = NULL;
	int prev_default;

	const char *node_name = NULL;
	const char *node_value = NULL;

	// libyang
	const struct lyd_node *node = NULL;

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
		error = sr_get_changes_iter(session, xpath, &changes_iterator);
		if (error != SR_ERR_OK) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_changes_iter() failed (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		while (sr_get_change_tree_next(session, changes_iterator, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
			// fetch node info
			node_name = LYD_NAME(node);
			node_value = lyd_get_value(node);

			// make sure we're reading the right node
			assert(strcmp(node_name, "location") == 0);

			// SRPLG_LOG_DBG(PLUGIN_NAME, "Node Path: %s", change_path);
			SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Value: %s; Operation: %d", node_name, node_value, operation);

			// don't do anything - keep location stored in the datastore, no need to apply anywhere
		}
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	return error;
}

int system_change_timezone_name(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;
	system_ctx_t *ctx = (system_ctx_t *) private_data;

	// sysrepo
	sr_change_iter_t *changes_iterator = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	const char *prev_value = NULL, *prev_list = NULL;
	int prev_default;

	const char *node_name = NULL;
	const char *node_value = NULL;

	// libyang
	const struct lyd_node *node = NULL;

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
		error = sr_get_changes_iter(session, xpath, &changes_iterator);
		if (error != SR_ERR_OK) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_changes_iter() failed (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		while (sr_get_change_tree_next(session, changes_iterator, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
			// fetch node info
			node_name = LYD_NAME(node);
			node_value = lyd_get_value(node);

			// make sure we're reading the right node
			assert(strcmp(node_name, "timezone-name") == 0);

			// SRPLG_LOG_DBG(PLUGIN_NAME, "Node Path: %s", change_path);
			SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Value: %s; Operation: %d", node_name, node_value, operation);

			switch (operation) {
				case SR_OP_CREATED:
				case SR_OP_MODIFIED:
					error = system_set_timezone_name(node_value);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "system_set_timezone_name() failed (%d)", error);
						goto error_out;
					}
					break;
				case SR_OP_DELETED:
					error = system_delete_timezone_name();
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "system_delete_timezone_name() failed (%d)", error);
						goto error_out;
					}
					break;
				case SR_OP_MOVED:
					break;
			}
		}
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;
out:
	return SR_ERR_CALLBACK_FAILED;
}

int system_change_timezone_utc_offset(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;
	system_ctx_t *ctx = (system_ctx_t *) private_data;

	// sysrepo
	sr_change_iter_t *changes_iterator = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	const char *prev_value = NULL, *prev_list = NULL;
	int prev_default;

	const char *node_name = NULL;
	const char *node_value = NULL;

	// libyang
	const struct lyd_node *node = NULL;

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
		error = sr_get_changes_iter(session, xpath, &changes_iterator);
		if (error != SR_ERR_OK) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_changes_iter() failed (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		while (sr_get_change_tree_next(session, changes_iterator, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
			// fetch node info
			node_name = LYD_NAME(node);
			node_value = lyd_get_value(node);

			// make sure we're reading the right node
			assert(strcmp(node_name, "timezone-utc-offset") == 0);

			// SRPLG_LOG_DBG(PLUGIN_NAME, "Node Path: %s", change_path);
			SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Value: %s; Operation: %d", node_name, node_value, operation);
		}
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;
out:
	return SR_ERR_CALLBACK_FAILED;
}

int system_change_ntp_enabled(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
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

int system_change_ntp_server(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
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

int system_change_dns_resolver_search(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = SR_ERR_OK;

	// sysrepo
	sr_change_iter_t *changes_iterator = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	const char *prev_value = NULL, *prev_list = NULL;
	int prev_default;

	const char *node_name = NULL;
	const char *node_value = NULL;

	// libyang
	const struct lyd_node *node = NULL;

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
		error = sr_get_changes_iter(session, xpath, &changes_iterator);
		if (error != SR_ERR_OK) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_changes_iter() failed (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		while (sr_get_change_tree_next(session, changes_iterator, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
			// fetch node info
			node_name = LYD_NAME(node);
			node_value = lyd_get_value(node);

			// make sure we're reading the right node
			assert(strcmp(node_name, "search") == 0);

			// SRPLG_LOG_DBG(PLUGIN_NAME, "Node Path: %s", change_path);
			SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Value: %s; Operation: %d", node_name, node_value, operation);

			switch (operation) {
				case SR_OP_CREATED:
					error = system_create_dns_search(node_value);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "system_create_dns_search() error (%d)", error);
						goto error_out;
					}
					break;
				case SR_OP_MODIFIED:
					// find current one and replace it
					break;
				case SR_OP_DELETED:
					error = system_delete_dns_search(node_value);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "system_delete_dns_search() error (%d)", error);
						goto error_out;
					}
					break;
				case SR_OP_MOVED:
					break;
			}
		}
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	return SR_ERR_CALLBACK_FAILED;
}

int system_change_dns_resolver_server(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
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
		// name change
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//name", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
			goto error_out;
		}
		error = system_apply_dns_server_change(ctx, session, xpath_buffer, system_change_dns_server_name);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_apply_dns_server_change() for name failed: %d", error);
			goto error_out;
		}

		// address change
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//address", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
			goto error_out;
		}
		error = system_apply_dns_server_change(ctx, session, xpath_buffer, system_change_dns_server_address);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_apply_dns_server_change() for address failed: %d", error);
			goto error_out;
		}

		// port change
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s//port", xpath);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
			goto error_out;
		}
		error = system_apply_dns_server_change(ctx, session, xpath_buffer, system_change_dns_server_port);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_apply_dns_server_change() for address failed: %d", error);
			goto error_out;
		}
	}

	goto out;
error_out:
	error = SR_ERR_CALLBACK_FAILED;
out:
	return SR_ERR_CALLBACK_FAILED;
}

int system_change_dns_resolver_timeout(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
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

int system_change_dns_resolver_attempts(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
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

int system_change_authentication_user_authentication_order(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
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

int system_change_authentication_user(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
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

static int system_set_timezone_name(const char *value)
{
	int error = 0;
	char path_buffer[PATH_MAX] = {0};

	error = snprintf(path_buffer, sizeof(path_buffer), "%s/%s", SYSTEM_TIMEZONE_DIR, value);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error (%d)", error);
		goto error_out;
	}

	error = access(path_buffer, F_OK);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "access() failed (%d)", error);
		goto error_out;
	}

	if (access(SYSTEM_LOCALTIME_FILE, F_OK) == 0) {
		error = unlink(SYSTEM_LOCALTIME_FILE);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "unlink() failed (%d)", error);
			goto error_out;
		}
	}

	error = symlink(path_buffer, SYSTEM_LOCALTIME_FILE);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "symlink() failed (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	return error;
}

static int system_delete_timezone_name(void)
{
	int error = 0;

	error = access(SYSTEM_LOCALTIME_FILE, F_OK);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "/etc/localtime doesn't exist");
		goto error_out;
	}

	error = unlink(SYSTEM_LOCALTIME_FILE);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unlink() failed (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	return error;
}

static int system_apply_dns_server_change(system_ctx_t *ctx, sr_session_ctx_t *session, const char *xpath, srpc_change_cb cb)
{
	int error = 0;

	// sysrepo
	sr_change_iter_t *changes_iterator = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	const char *prev_value = NULL, *prev_list = NULL;
	int prev_default;

	// libyang
	const struct lyd_node *node = NULL;

	error = sr_get_changes_iter(session, xpath, &changes_iterator);
	if (error != SR_ERR_OK) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_changes_iter() failed (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	while (sr_get_change_tree_next(session, changes_iterator, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
		error = cb(ctx, session, prev_value, node, operation);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "callback failed for xpath %s", xpath);
			goto error_out;
		}
	}

	goto out;

error_out:
	error = -1;

out:
	return error;
}

static int system_change_dns_server_name(void *priv, sr_session_ctx_t *session, const char *prev_value, const struct lyd_node *node, sr_change_oper_t operation)
{
	int error = 0;
	const char *node_name = NULL;
	const char *node_value = NULL;

	node_name = LYD_NAME(node);
	node_value = lyd_get_value(node);

	assert(strcmp(node_name, "name") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Value: %s; Operation: %d", node_name, node_value, operation);

	return error;
}

static int system_change_dns_server_address(void *priv, sr_session_ctx_t *session, const char *prev_value, const struct lyd_node *node, sr_change_oper_t operation)
{
	int error = 0;

	const char *node_name = NULL;
	const char *node_value = NULL;

	node_name = LYD_NAME(node);
	node_value = lyd_get_value(node);

	assert(strcmp(node_name, "address") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Value: %s; Operation: %d", node_name, node_value, operation);

	switch (operation) {
		case SR_OP_CREATED:
			error = system_create_dns_server_address(node_value);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_add_dns_server() error (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_MODIFIED:
			error = system_modify_dns_server_address(prev_value, node_value);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_add_dns_server() error (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_DELETED:
			// delete from the system based on the address value
			error = system_delete_dns_server_address(node_value);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_delete_dns_server() error (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_MOVED:
			break;
	}

	goto out;

error_out:
	error = -1;

out:

	return error;
}

static int system_change_dns_server_port(void *priv, sr_session_ctx_t *session, const char *prev_value, const struct lyd_node *node, sr_change_oper_t operation)
{
	int error = 0;

	const char *node_name = NULL;
	const char *node_value = NULL;

	node_name = LYD_NAME(node);
	node_value = lyd_get_value(node);

	assert(strcmp(node_name, "port") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Value: %s; Operation: %d", node_name, node_value, operation);

	// unsupported for now

	return error;
}