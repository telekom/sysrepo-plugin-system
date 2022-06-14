#include "change.h"
#include "common.h"

#include "libyang/tree_data.h"
#include "sysrepo/xpath.h"
#include "sysrepo_types.h"
#include "system/data/ntp/server.h"
#include "system/data/ntp/server/list.h"

#include <assert.h>
#include <sysrepo.h>

static int system_ntp_load_server_node_address(sr_session_ctx_t *session, const struct lyd_node *node, char *address_buffer, size_t buffer_size);

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
	system_ctx_t *ctx = priv;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);
	system_ntp_server_t temp_server = {0};
	system_ntp_server_element_t *found_server_el = NULL;

	assert(strcmp(node_name, "address") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	system_ntp_server_init(&temp_server);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			// set name and address to the same value - real datastore name is not used on the system
			error = system_ntp_server_set_name(&temp_server, node_value);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_name() error (%d)", error);
				goto error_out;
			}
			error = system_ntp_server_set_address(&temp_server, node_value);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_address() error (%d)", error);
				goto error_out;
			}

			// add the new server to the list
			error = system_ntp_server_list_add(&ctx->temp_ntp_servers, temp_server);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_list_add() error (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_MODIFIED:
			// get existing server and change address
			found_server_el = system_ntp_server_list_find(ctx->temp_ntp_servers, node_value);
			if (!found_server_el) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_list_find() error");
				goto error_out;
			}

			// name
			error = system_ntp_server_set_name(&found_server_el->server, node_value);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_name() error (%d)", error);
				goto error_out;
			}

			// address
			error = system_ntp_server_set_address(&found_server_el->server, node_value);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_address() error (%d)", error);
				goto error_out;
			}

			break;
		case SR_OP_DELETED:
			// remove data from list
			error = system_ntp_server_list_remove(&ctx->temp_ntp_servers, node_value);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_server_list_remove() error (%d)", error);
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

int system_ntp_change_server_port(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);
	char address_buffer[100] = {0};

	assert(strcmp(node_name, "port") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	error = system_ntp_load_server_node_address(session, change_ctx->node, address_buffer, sizeof(address_buffer));
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_load_server_node_address() error (%d)", error);
		goto error_out;
	}

	SRPLG_LOG_DBG(PLUGIN_NAME, "ADDRESS = %s, PORT=100", address_buffer);

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

	goto out;

error_out:
	error = -1;

out:
	return error;
}

int system_ntp_change_server_association_type(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	system_ctx_t *ctx = priv;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);
	char address_buffer[100] = {0};
	system_ntp_server_element_t *found_server_el = NULL;

	assert(strcmp(node_name, "association-type") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	error = system_ntp_load_server_node_address(session, change_ctx->node, address_buffer, sizeof(address_buffer));
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_load_server_node_address() error (%d)", error);
		goto error_out;
	}

	SRPLG_LOG_DBG(PLUGIN_NAME, "Changing association-type for server %s", address_buffer);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
		case SR_OP_MODIFIED:
			// find server
			found_server_el = system_ntp_server_list_find(ctx->temp_ntp_servers, address_buffer);
			if (!found_server_el) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_list_find() failed");
				goto error_out;
			}

			// change value
			error = system_ntp_server_set_association_type(&found_server_el->server, node_value);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_association_type() error (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_DELETED:
			// nothing to do, only remove at address
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

int system_ntp_change_server_iburst(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	system_ctx_t *ctx = priv;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);
	char address_buffer[100] = {0};
	system_ntp_server_element_t *found_server_el = NULL;

	assert(strcmp(node_name, "iburst") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	error = system_ntp_load_server_node_address(session, change_ctx->node, address_buffer, sizeof(address_buffer));
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_load_server_node_address() error (%d)", error);
		goto error_out;
	}

	SRPLG_LOG_DBG(PLUGIN_NAME, "Changing iburst for server %s", address_buffer);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
		case SR_OP_MODIFIED:
			// find server
			found_server_el = system_ntp_server_list_find(ctx->temp_ntp_servers, address_buffer);
			if (!found_server_el) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_list_find() failed");
				goto error_out;
			}

			// change value
			error = system_ntp_server_set_iburst(&found_server_el->server, node_value);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_iburst() error (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_DELETED:
			// nothing to do, only remove at address
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

int system_ntp_change_server_prefer(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	system_ctx_t *ctx = priv;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);
	char address_buffer[100] = {0};
	system_ntp_server_element_t *found_server_el = NULL;

	assert(strcmp(node_name, "prefer") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	error = system_ntp_load_server_node_address(session, change_ctx->node, address_buffer, sizeof(address_buffer));
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_load_server_node_address() error (%d)", error);
		goto error_out;
	}

	SRPLG_LOG_DBG(PLUGIN_NAME, "Changing prefer for server %s", address_buffer);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
		case SR_OP_MODIFIED:
			// find server
			found_server_el = system_ntp_server_list_find(ctx->temp_ntp_servers, address_buffer);
			if (!found_server_el) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_list_find() failed");
				goto error_out;
			}

			// change value
			error = system_ntp_server_set_prefer(&found_server_el->server, node_value);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ntp_server_set_prefer() error (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_DELETED:
			// nothing to do, only remove at address
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

static int system_ntp_load_server_node_address(sr_session_ctx_t *session, const struct lyd_node *node, char *address_buffer, size_t buffer_size)
{
	int error = 0;
	char path_buffer[PATH_MAX] = {0};
	char address_path_buffer[PATH_MAX] = {0};

	// sysrepo data
	sr_val_t *address_value = NULL;
	sr_xpath_ctx_t xpath_ctx = {0};
	const char *server_name = NULL;

	// get node full path
	error = (lyd_path(node, LYD_PATH_STD, path_buffer, sizeof(path_buffer)) == NULL);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_path() failed");
		goto error_out;
	}

	// extract key
	server_name = sr_xpath_key_value(path_buffer, "server", "name", &xpath_ctx);
	if (!server_name) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_xpath_key_value() failed");
		goto error_out;
	}

	// fetch address of the current server

	// create path
	error = snprintf(address_path_buffer, sizeof(address_path_buffer), "/ietf-system:system/ntp/server[name=\"%s\"]/udp/address", server_name);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed");
		goto error_out;
	}

	// fetch value
	error = sr_get_item(session, address_path_buffer, 0, &address_value);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_item() error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	// assert the address is a string for getting string data
	assert(address_value->type == SR_STRING_T);

	// store value in the provided buffer
	error = snprintf(address_buffer, buffer_size, "%s", address_value->data.string_val);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed");
		goto error_out;
	}

	error = 0;
	goto out;

error_out:
	error = -1;

out:
	// free gotten value
	sr_free_val(address_value);

	return error;
}