#include "load.h"

#include <sysrepo.h>

#include <srpc.h>

#include <utlist.h>

// core library
#include "core/ly_tree.h"
#include "core/api/system/ntp/load.h"
#include "core/data/system/ntp/server/list.h"

static int system_aug_running_load_hostname(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);
static int system_aug_running_load_ntp(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);

int system_aug_running_ds_load(system_ctx_t *ctx, sr_session_ctx_t *session)
{
	int error = 0;

	const struct ly_ctx *ly_ctx = NULL;
	struct lyd_node *system_container_node = NULL;
	sr_conn_ctx_t *conn_ctx = NULL;

	srpc_startup_load_t load_values[] = {
		{
			"hostname",
			system_aug_running_load_hostname,
		},
		{
			"ntp",
			system_aug_running_load_ntp,
		},
	};

	conn_ctx = sr_session_get_connection(session);
	ly_ctx = sr_acquire_context(conn_ctx);
	if (ly_ctx == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to get ly_ctx variable");
		goto error_out;
	}

	// reload features hash before adding all system values
	SRPC_SAFE_CALL_ERR(error, srpc_feature_status_hash_reload(&ctx->ietf_system_features, session, IETF_SYSTEM_YANG_MODULE), error_out);

	// load system container info
	error = system_ly_tree_create_system(ly_ctx, &system_container_node);
	for (size_t i = 0; i < ARRAY_SIZE(load_values); i++) {
		const srpc_startup_load_t *load = &load_values[i];

		error = load->cb((void *) ctx, session, ly_ctx, system_container_node);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "Node creation callback failed for value %s", load->name);
			goto error_out;
		}
	}

// enable or disable storing into startup - use when testing load functionality for now
#define SYSTEM_PLUGIN_LOAD_STARTUP

#ifdef SYSTEM_PLUGIN_LOAD_STARTUP
	error = sr_edit_batch(session, system_container_node, "merge");
	if (error != SR_ERR_OK) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_edit_batch() error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	error = sr_apply_changes(session, 0);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_apply_changes() error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}
#endif

	goto out;

error_out:
	error = -1;

out:
	if (system_container_node) {
		lyd_free_tree(system_container_node);
	}

	sr_release_context(conn_ctx);

	return error;
}

static int system_aug_running_load_hostname(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node)
{
}

static int system_aug_running_load_ntp(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node)
{
	int error = 0;

	system_ctx_t *ctx = priv;

	// ietf-system nodes
	struct lyd_node *ntp_container_node = NULL, *server_list_node = NULL;

	// feature check
	bool ntp_enabled = srpc_feature_status_hash_check(ctx->ietf_system_features, "ntp");
	bool ntp_udp_port_enabled = srpc_feature_status_hash_check(ctx->ietf_system_features, "ntp-udp-port");

	// load list
	system_ntp_server_element_t *ntp_server_head = NULL, *ntp_server_iter = NULL;

	SRPLG_LOG_INF(PLUGIN_NAME, "Loading NTP data");

	if (ntp_enabled) {
		error = system_ly_tree_create_ntp(ly_ctx, parent_node, &ntp_container_node);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_ntp() error (%d)", error);
			goto error_out;
		}

		// load system values
		system_ntp_server_list_init(&ntp_server_head);
		error = system_ntp_load_server(ctx, &ntp_server_head);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_load_server() error (%d)", error);
			goto error_out;
		}

		LL_FOREACH(ntp_server_head, ntp_server_iter)
		{
			// name
			error = system_ly_tree_create_ntp_server(ly_ctx, ntp_container_node, &server_list_node, ntp_server_iter->server.name);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_ntp_server() error (%d)", error);
				goto error_out;
			}

			SRPLG_LOG_INF(PLUGIN_NAME, "Setting address %s", ntp_server_iter->server.address);

			// address
			error = system_ly_tree_create_ntp_server_address(ly_ctx, server_list_node, ntp_server_iter->server.address);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_ntp_server_address() error (%d)", error);
				goto error_out;
			}

			SRPLG_LOG_INF(PLUGIN_NAME, "Setting port \"%s\"", ntp_server_iter->server.port);

			// port
			if (ntp_server_iter->server.port && ntp_udp_port_enabled) {
				error = system_ly_tree_create_ntp_server_port(ly_ctx, server_list_node, ntp_server_iter->server.port);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_ntp_server_port() error (%d)", error);
					goto error_out;
				}
			}

			// association type
			error = system_ly_tree_create_ntp_server_association_type(ly_ctx, server_list_node, ntp_server_iter->server.association_type);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_ntp_server_association_type() error (%d)", error);
				goto error_out;
			}

			// iburst
			if (ntp_server_iter->server.iburst) {
				error = system_ly_tree_create_ntp_server_iburst(ly_ctx, server_list_node, ntp_server_iter->server.iburst);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_ntp_server_iburst() error (%d)", error);
					goto error_out;
				}
			}

			// prefer
			if (ntp_server_iter->server.prefer) {
				error = system_ly_tree_create_ntp_server_prefer(ly_ctx, server_list_node, ntp_server_iter->server.prefer);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_ntp_server_prefer() error (%d)", error);
					goto error_out;
				}
			}
		}

		goto out;
	}

error_out:
	error = -1;

out:

	system_ntp_server_list_free(&ntp_server_head);

	return error;
}