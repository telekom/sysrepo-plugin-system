#include "change.h"
#include "common.h"

#include <assert.h>
#include <sysrepo.h>

int system_ntp_change_enabled(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);

	assert(strcmp(node_name, "enabled") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			break;
		case SR_OP_MODIFIED:
			break;
		case SR_OP_DELETED:
			break;
		case SR_OP_MOVED:
			break;
	}

	return error;
}

int system_ntp_change_server_name(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);

	assert(strcmp(node_name, "name") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			break;
		case SR_OP_MODIFIED:
			break;
		case SR_OP_DELETED:
			break;
		case SR_OP_MOVED:
			break;
	}

	return error;
}

int system_ntp_change_server_address(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);

	assert(strcmp(node_name, "address") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			break;
		case SR_OP_MODIFIED:
			break;
		case SR_OP_DELETED:
			break;
		case SR_OP_MOVED:
			break;
	}

	return error;
}

int system_ntp_change_server_port(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);

	assert(strcmp(node_name, "port") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			break;
		case SR_OP_MODIFIED:
			break;
		case SR_OP_DELETED:
			break;
		case SR_OP_MOVED:
			break;
	}

	return error;
}

int system_ntp_change_server_association_type(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);

	assert(strcmp(node_name, "association-type") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			break;
		case SR_OP_MODIFIED:
			break;
		case SR_OP_DELETED:
			break;
		case SR_OP_MOVED:
			break;
	}

	return error;
}

int system_ntp_change_server_iburst(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);

	assert(strcmp(node_name, "iburst") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			break;
		case SR_OP_MODIFIED:
			break;
		case SR_OP_DELETED:
			break;
		case SR_OP_MOVED:
			break;
	}

	return error;
}

int system_ntp_change_server_prefer(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);

	assert(strcmp(node_name, "prefer") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			break;
		case SR_OP_MODIFIED:
			break;
		case SR_OP_DELETED:
			break;
		case SR_OP_MOVED:
			break;
	}

	return error;
}
