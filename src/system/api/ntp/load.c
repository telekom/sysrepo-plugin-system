#include "load.h"
#include "common.h"

// data
#include "system/data/ntp/server.h"
#include "system/data/ntp/server/list.h"

#include <assert.h>
#include <string.h>
#include <sysrepo.h>
#include <srpc.h>

int system_ntp_load_server(system_ctx_t *ctx, system_ntp_server_element_t **head)
{
	int error = 0;

	sr_data_t *subtree = NULL;

	// temp values
	system_ntp_server_t temp_server = {0};
	const char *full_addr = NULL, *address = NULL, *port = NULL, *delimiter = NULL;

	// ntp config nodes
	struct lyd_node *config_entry_node = NULL, *server_node = NULL, *peer_node = NULL, *pool_node = NULL, *chosen_node = NULL, *word_node = NULL;

	// NTP server options (iburst and prefer)
	struct lyd_node *options_entry_node = NULL, *iburst_node = NULL, *prefer_node = NULL;

	// get ntp config startup data
	error = sr_get_subtree(ctx->startup_session, "/ntp:ntp[config-file=\'/etc/ntp.conf\']", 0, &subtree);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_subtree() error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	// iterate config file data and apply to startup DS
	config_entry_node = srpc_ly_tree_get_child_list(subtree->tree, "config-entries");
	while (config_entry_node) {
		// entry can be either server, pool or peer
		server_node = srpc_ly_tree_get_child_container(config_entry_node, "server");
		pool_node = srpc_ly_tree_get_child_container(config_entry_node, "pool");
		peer_node = srpc_ly_tree_get_child_container(config_entry_node, "peer");

		if (server_node || pool_node || peer_node) {
			system_ntp_server_init(&temp_server);

			if (server_node) {
				chosen_node = server_node;
			} else if (pool_node) {
				chosen_node = pool_node;
			} else if (peer_node) {
				chosen_node = peer_node;
			}

			word_node = srpc_ly_tree_get_child_leaf(chosen_node, "word");

			assert(word_node != NULL);

			error = system_ntp_server_set_name(&temp_server, lyd_get_value(word_node));
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_name() error (%d)", error);
				goto error_out;
			}

			full_addr = lyd_get_value(word_node);
			if ((delimiter = strchr(full_addr, ':')) != NULL) {
				// address contains a port - split and use both
				port = delimiter + 1;
				address = strndup(full_addr, (size_t) (delimiter - full_addr));
				error = system_ntp_server_set_address(&temp_server, address);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_address() error (%d)", error);
					goto error_out;
				}
				error = system_ntp_server_set_port(&temp_server, port);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_port() error (%d)", error);
					goto error_out;
				}
				free((char *) address);
			} else {
				error = system_ntp_server_set_address(&temp_server, full_addr);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_address() error (%d)", error);
					goto error_out;
				}
			}

			error = system_ntp_server_set_association_type(&temp_server, LYD_NAME(chosen_node));
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_association_type() error (%d)", error);
				goto error_out;
			}

			options_entry_node = srpc_ly_tree_get_child_list(chosen_node, "config-entries");
			if (options_entry_node) {
				// iterate options and apply to the server node
				while (options_entry_node) {
					iburst_node = srpc_ly_tree_get_child_leaf(options_entry_node, "iburst");
					prefer_node = srpc_ly_tree_get_child_leaf(options_entry_node, "prefer");

					// iburst
					if (iburst_node) {
						error = system_ntp_server_set_iburst(&temp_server, "true");
						if (error) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_iburst() error (%d)", error);
							goto error_out;
						}
					}

					// prefer
					if (prefer_node) {
						error = system_ntp_server_set_prefer(&temp_server, "true");
						if (error) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_prefer() error (%d)", error);
							goto error_out;
						}
					}
					options_entry_node = srpc_ly_tree_get_list_next(options_entry_node);
				}
			}

			// add the element to the list
			error = system_ntp_server_list_add(head, temp_server);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_list_add() error (%d)", error);
				goto error_out;
			}

			// free temporary server
			system_ntp_server_free(&temp_server);
		}
		config_entry_node = srpc_ly_tree_get_list_next(config_entry_node);
	}

	goto out;
error_out:
	error = -1;

out:
	if (subtree) {
		sr_release_data(subtree);
	}

	// free if load interrupted
	system_ntp_server_free(&temp_server);
	return error;
}