#include "startup.h"
#include "common.h"
#include "ly_tree.h"

#include <sysrepo.h>

// helpers

typedef int (*system_startup_load_cb)(system_ctx_t *ctx, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);

static int system_startup_load_hostname(system_ctx_t *ctx, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);
static int system_startup_load_contact(system_ctx_t *ctx, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);
static int system_startup_load_location(system_ctx_t *ctx, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);
static int system_startup_load_timezone_name(system_ctx_t *ctx, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);

////

int system_startup_load_data(system_ctx_t *ctx, sr_session_ctx_t *session)
{
	int error = 0;

	const struct ly_ctx *ly_ctx = NULL;
	struct lyd_node *system_container_node = NULL;
	sr_conn_ctx_t *conn_ctx = NULL;

	system_startup_load_cb system_container_callbacks[] = {
		system_startup_load_hostname,
		system_startup_load_contact,
		system_startup_load_location,
		system_startup_load_timezone_name,
	};

	conn_ctx = sr_session_get_connection(session);
	ly_ctx = sr_acquire_context(conn_ctx);
	if (ly_ctx == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to get ly_ctx variable");
		goto error_out;
	}

	// load system container info
	error = system_ly_tree_create_system_container(ly_ctx, &system_container_node);
	for (size_t i = 0; i < ARRAY_SIZE(system_container_callbacks); i++) {
		error = system_container_callbacks[i](ctx, session, ly_ctx, system_container_node);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "Node creation callback for system container node failed");
			goto error_out;
		}
	}

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

	goto out;

error_out:
	error = -1;

out:
	sr_release_context(conn_ctx);
	return error;
}

static int system_startup_load_hostname(system_ctx_t *ctx, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node)
{
	int error = 0;
	return error;
}

static int system_startup_load_contact(system_ctx_t *ctx, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node)
{
	int error = 0;
	return error;
}

static int system_startup_load_location(system_ctx_t *ctx, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node)
{
	int error = 0;
	return error;
}

static int system_startup_load_timezone_name(system_ctx_t *ctx, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node)
{
	int error = 0;
	return error;
}