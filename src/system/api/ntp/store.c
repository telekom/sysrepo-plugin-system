#include "store.h"
#include "common.h"
#include "context.h"
#include "libyang/printer_data.h"
#include "srpc/ly_tree.h"
#include "types.h"

#include <assert.h>
#include <sysrepo.h>
#include <srpc.h>
#include <utlist.h>

int system_ntp_store_server(system_ctx_t *ctx, system_ntp_server_element_t *head)
{
	int error = 0;

	// config nodes
	const struct ly_ctx *ly_ctx = NULL;
	struct lyd_node *ntp_list_node = NULL, *config_entry_node = NULL, *server_node = NULL;
	struct lyd_node *options_entry_node = NULL;
	sr_conn_ctx_t *conn_ctx = NULL;
	char id_buffer[100] = {0};
	char full_address_buffer[100] = {0};

	size_t id, option_id;
	system_ntp_server_element_t *iter = NULL;

	conn_ctx = sr_session_get_connection(ctx->startup_session);
	ly_ctx = sr_acquire_context(conn_ctx);
	if (ly_ctx == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to get ly_ctx variable");
		goto error_out;
	}

	error = srpc_ly_tree_create_list(ly_ctx, NULL, &ntp_list_node, "/ntp:ntp", "config-file", "/etc/ntp.conf");
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_ly_tree_create_list() error (%d)", error);
	}

	id = 1;
	LL_FOREACH(head, iter)
	{
		SRPLG_LOG_DBG(PLUGIN_NAME, "Adding NTP server %s", iter->server.name);

		// 1. create config entry
		SRPLG_LOG_DBG(PLUGIN_NAME, "Creating new config-entries list node");
		error = snprintf(id_buffer, sizeof(id_buffer), "%lu", id);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error (%d)", error);
			goto error_out;
		}
		error = srpc_ly_tree_create_list(ly_ctx, ntp_list_node, &config_entry_node, "config-entries", "_id", id_buffer);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_ly_tree_create_list() error (%d)", error);
			goto error_out;
		}

		// 2. add pool | server | peer node
		SRPLG_LOG_DBG(PLUGIN_NAME, "Creating new server list node");
		assert(
			strcmp(iter->server.association_type, "server") == 0 ||
			strcmp(iter->server.association_type, "pool") == 0 ||
			strcmp(iter->server.association_type, "peer") == 0);
		error = srpc_ly_tree_create_container(ly_ctx, config_entry_node, &server_node, iter->server.association_type);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_ly_tree_create_container() error (%d)", error);
			goto error_out;
		}

		// 3. set word (address:port) to the server
		SRPLG_LOG_DBG(PLUGIN_NAME, "Setting server list address and port");
		if (iter->server.port) {
			error = snprintf(full_address_buffer, sizeof(full_address_buffer), "%s:%s", iter->server.address, iter->server.port);
			if (error < 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error (%d)", error);
				goto error_out;
			}
		} else {
			error = snprintf(full_address_buffer, sizeof(full_address_buffer), "%s", iter->server.address);
			if (error < 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error (%d)", error);
				goto error_out;
			}
		}

		error = srpc_ly_tree_create_leaf(ly_ctx, server_node, NULL, "word", full_address_buffer);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_ly_tree_create_leaf() error (%d)", error);
			goto error_out;
		}

		// 4. setup properties (iburst and prefer)
		SRPLG_LOG_DBG(PLUGIN_NAME, "Adding iburst and prefer options");
		option_id = 1;

		if (iter->server.iburst) {
			if (!strcmp(iter->server.iburst, "true")) {
				SRPLG_LOG_DBG(PLUGIN_NAME, "Adding iburst options for server %s", iter->server.name);
				error = snprintf(id_buffer, sizeof(id_buffer), "%lu", option_id);
				if (error < 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error (%d)", error);
					goto error_out;
				}
				error = srpc_ly_tree_create_list(ly_ctx, server_node, &options_entry_node, "config-entries", "_id", id_buffer);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_ly_tree_create_list() error (%d)", error);
					goto error_out;
				}

				error = srpc_ly_tree_create_leaf(ly_ctx, options_entry_node, NULL, "iburst", NULL);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_ly_tree_create_leaf() error (%d)", error);
					goto error_out;
				}

				++option_id;
			}
		}

		if (iter->server.prefer) {
			if (!strcmp(iter->server.prefer, "true")) {
				SRPLG_LOG_DBG(PLUGIN_NAME, "Adding prefer options for server %s", iter->server.name);
				error = snprintf(id_buffer, sizeof(id_buffer), "%lu", option_id);
				if (error < 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error (%d)", error);
					goto error_out;
				}
				error = srpc_ly_tree_create_list(ly_ctx, server_node, &options_entry_node, "config-entries", "_id", id_buffer);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_ly_tree_create_list() error (%d)", error);
					goto error_out;
				}

				error = srpc_ly_tree_create_leaf(ly_ctx, options_entry_node, NULL, "prefer", NULL);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_ly_tree_create_leaf() error (%d)", error);
					goto error_out;
				}
			}
		}

		++id;
	}

	// apply changes to the config file
	SRPLG_LOG_INF(PLUGIN_NAME, "Applying created changes to the /etc/ntp.conf config file");

	// lyd_print_file(stdout, ntp_list_node, LYD_XML, 0);

	error = sr_edit_batch(ctx->startup_session, ntp_list_node, "merge");
	if (error != SR_ERR_OK) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_edit_batch() error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	error = sr_apply_changes(ctx->startup_session, 0);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_apply_changes() error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Successfully applied /etc/ntp.conf config file changes");

	goto out;

error_out:
	error = -1;

out:
	if (ntp_list_node) {
		lyd_free_tree(ntp_list_node);
	}
	sr_release_context(conn_ctx);
	return error;
}