#include "store.h"

#include <sysrepo.h>
#include <srpc.h>
#include <utlist.h>

// core library
#include "core/ly_tree.h"
#include "core/api/system/ntp/store.h"
#include "core/api/system/ntp/check.h"
#include "core/api/system/store.h"
#include "core/api/system/check.h"

// core data
#include "core/data/system/ntp/server/list.h"
#include "core/data/system/ntp/server.h"

static int system_aug_running_store_hostname(void *priv, const struct lyd_node *system_container_node);
static int system_aug_running_store_ntp(void *priv, const struct lyd_node *system_container_node);

int system_aug_running_ds_store(system_ctx_t *ctx, sr_session_ctx_t *session)
{
	int error = 0;
	sr_data_t *subtree = NULL;

	error = sr_get_subtree(session, SYSTEM_SYSTEM_CONTAINER_YANG_PATH, 0, &subtree);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_subtree() error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	srpc_startup_store_t store_values[] = {
		{
			"hostname",
			system_aug_running_store_hostname,
		},
		{
			"ntp",
			system_aug_running_store_ntp,
		},
	};

	// reload feature status hash before storing system data
	SRPC_SAFE_CALL_ERR(error, srpc_feature_status_hash_reload(&ctx->ietf_system_features, session, IETF_SYSTEM_YANG_MODULE), error_out);

	for (size_t i = 0; i < ARRAY_SIZE(store_values); i++) {
		const srpc_startup_store_t *store = &store_values[i];

		error = store->cb(ctx, subtree->tree);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "Startup store callback failed for value %s", store->name);
			goto error_out;
		}
	}

	goto out;

error_out:
	error = -1;

out:
	if (subtree) {
		sr_release_data(subtree);
	}

	return error;
}

static int system_aug_running_store_hostname(void *priv, const struct lyd_node *system_container_node)
{
	int error = 0;
	system_ctx_t *ctx = (system_ctx_t *) priv;
	srpc_check_status_t check_status = srpc_check_status_none;

	struct lyd_node *hostname_node = srpc_ly_tree_get_child_leaf(system_container_node, "hostname");

	if (hostname_node) {
		const char *hostname = lyd_get_value(hostname_node);

		SRPLG_LOG_INF(PLUGIN_NAME, "Checking system hostname value");
		check_status = system_check_hostname(ctx, hostname);

		switch (check_status) {
			case srpc_check_status_none:
				SRPLG_LOG_ERR(PLUGIN_NAME, "Error loading current system hostname value");
				goto error_out;
				break;
			case srpc_check_status_error:
				SRPLG_LOG_ERR(PLUGIN_NAME, "Error loading current system hostname value");
				goto error_out;
				break;
			case srpc_check_status_non_existant:
				SRPLG_LOG_INF(PLUGIN_NAME, "Storing hostname value %s", hostname);

				error = system_store_hostname(ctx, hostname);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_store_hostname() failed (%d)", error);
					return -1;
				}
				break;
			case srpc_check_status_equal:
				SRPLG_LOG_ERR(PLUGIN_NAME, "Startup hostname value is already applied on the system");
				break;
			case srpc_check_status_partial:
				// should not be returned - treat as error
				SRPLG_LOG_ERR(PLUGIN_NAME, "Error loading current system hostname value");
				goto error_out;
				break;
		}
	}

	goto out;

error_out:
	error = -1;

out:
	return 0;
}

static int system_aug_running_store_ntp(void *priv, const struct lyd_node *system_container_node)
{
	int error = 0;

	system_ctx_t *ctx = (system_ctx_t *) priv;

	struct lyd_node *ntp_container_node = NULL;
	struct lyd_node *server_list_node = NULL;
	struct lyd_node *server_name_leaf_node = NULL;
	struct lyd_node *server_address_leaf_node = NULL;
	struct lyd_node *server_port_leaf_node = NULL;
	struct lyd_node *server_association_type_leaf_node = NULL;
	struct lyd_node *server_iburst_leaf_node = NULL, *server_prefer_leaf_node = NULL;
	struct lyd_node *udp_container_node = NULL;

	system_ntp_server_element_t *ntp_server_head = NULL;

	bool ntp_enabled = srpc_feature_status_hash_check(ctx->ietf_system_features, "ntp");
	bool ntp_udp_port_enabled = srpc_feature_status_hash_check(ctx->ietf_system_features, "ntp-udp-port");

	system_ntp_server_t temp_server = {0};
	srpc_check_status_t server_check_status = srpc_check_status_none;

	if (ntp_enabled) {
		SRPLG_LOG_INF(PLUGIN_NAME, "Storing NTP startup data");

		ntp_container_node = srpc_ly_tree_get_child_container(system_container_node, "ntp");
		if (ntp_container_node) {
			server_list_node = srpc_ly_tree_get_child_list(ntp_container_node, "server");

			if (server_list_node) {
				while (server_list_node) {
					// process server list node
					system_ntp_server_init(&temp_server);

					server_name_leaf_node = srpc_ly_tree_get_child_leaf(server_list_node, "name");
					udp_container_node = srpc_ly_tree_get_child_container(server_list_node, "udp");
					server_association_type_leaf_node = srpc_ly_tree_get_child_leaf(server_list_node, "association-type");
					server_iburst_leaf_node = srpc_ly_tree_get_child_leaf(server_list_node, "iburst");
					server_prefer_leaf_node = srpc_ly_tree_get_child_leaf(server_list_node, "prefer");

					const char *name = lyd_get_value(server_name_leaf_node);

					// set name
					system_ntp_server_set_name(&temp_server, name);

					if (!udp_container_node) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_ly_tree_get_child_container() failed for udp");
						goto error_out;
					}

					server_address_leaf_node = srpc_ly_tree_get_child_leaf(udp_container_node, "address");

					// address
					if (server_address_leaf_node) {
						const char *address = lyd_get_value(server_address_leaf_node);

						// set address
						error = system_ntp_server_set_address(&temp_server, address);
						if (error) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_address() error (%d)", error);
							goto error_out;
						}
					} else {
						// no address node -> unable to continue
						SRPLG_LOG_INF(PLUGIN_NAME, "srpc_ly_tree_get_child_leaf() failed for leaf address");
						goto error_out;
					}

					if (ntp_udp_port_enabled) {
						server_port_leaf_node = srpc_ly_tree_get_child_leaf(udp_container_node, "port");

						// port
						if (server_port_leaf_node) {
							const char *port = lyd_get_value(server_port_leaf_node);

							// set port
							error = system_ntp_server_set_port(&temp_server, port);
							if (error) {
								SRPLG_LOG_INF(PLUGIN_NAME, "system_ntp_server_set_port() error (%d)", error);
								goto error_out;
							}
						}
					}

					// association-type
					if (server_association_type_leaf_node) {
						const char *association_type = lyd_get_value(server_association_type_leaf_node);

						error = system_ntp_server_set_association_type(&temp_server, association_type);
						if (error) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_association_type() error (%d)", error);
							goto error_out;
						}
					} else {
						// unable to create config entry without association type
						SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_ly_tree_get_child_leaf() failed for leaf association-type");
						goto error_out;
					}

					// iburst
					if (server_iburst_leaf_node) {
						const char *iburst = lyd_get_value(server_iburst_leaf_node);

						error = system_ntp_server_set_iburst(&temp_server, iburst);
						if (error) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_iburst() error (%d)", error);
							goto error_out;
						}
					}

					// prefer
					if (server_prefer_leaf_node) {
						const char *prefer = lyd_get_value(server_prefer_leaf_node);

						error = system_ntp_server_set_prefer(&temp_server, prefer);
						if (error) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_prefer() error (%d)", error);
							goto error_out;
						}
					}

					// append to the list
					error = system_ntp_server_list_add(&ntp_server_head, temp_server);
					if (error) {
						SRPLG_LOG_INF(PLUGIN_NAME, "system_ntp_server_list_add() error (%d)", error);
						goto error_out;
					}

					system_ntp_server_free(&temp_server);

					// iterate list
					server_list_node = srpc_ly_tree_get_list_next(server_list_node);
				}
			}

			SRPLG_LOG_INF(PLUGIN_NAME, "Checking NTP server list status on the system");

			server_check_status = system_ntp_check_server(ctx, ntp_server_head);

			SRPLG_LOG_INF(PLUGIN_NAME, "Recieved check status: %d", server_check_status);

			switch (server_check_status) {
				case srpc_check_status_none:
					SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured while checking NTP server system values");
					goto error_out;
					break;
				case srpc_check_status_error:
					SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured while checking NTP server system values");
					goto error_out;
					break;
				case srpc_check_status_non_existant:
					SRPLG_LOG_INF(PLUGIN_NAME, "NTP server list values don\'t exist on the system - applying values");

					error = system_ntp_store_server(ctx, ntp_server_head);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_store_server() error (%d)", error);
						goto error_out;
					}

					SRPLG_LOG_INF(PLUGIN_NAME, "Applied NTP server startup values to the system");
					break;
				case srpc_check_status_equal:
					SRPLG_LOG_INF(PLUGIN_NAME, "NTP server startup values already exist on the system - no need to apply anything");
					break;
				case srpc_check_status_partial:
					// TODO: implement
					break;
			}
		}
	} else {
		SRPLG_LOG_INF(PLUGIN_NAME, "\"ntp\" feature disabled - skipping NTP startup configuration");
	}

	goto out;

error_out:
	error = -1;

out:
	// remove temp values if something was interrupted
	system_ntp_server_free(&temp_server);

	// free allocated lists
	system_ntp_server_list_free(&ntp_server_head);

	return error;
}
