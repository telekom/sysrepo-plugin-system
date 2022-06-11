#include "change.h"
#include "libyang/tree_data.h"
#include "load.h"
#include "srpc/node.h"
#include "srpc/types.h"
#include "store.h"
#include "sysrepo_types.h"
#include "types.h"
#include "common.h"

// data
#include "system/data/dns_resolver/server/list.h"
#include "system/data/dns_resolver/search/list.h"
#include "system/data/dns_resolver/search.h"
#include "system/data/dns_resolver/server.h"
#include "system/data/ip_address.h"

#include <sysrepo.h>

#include <utlist.h>

int system_dns_resolver_change_search(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	system_ctx_t *ctx = priv;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);
	system_dns_search_t temp_search = {0};

	assert(strcmp(node_name, "search") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	system_dns_search_init(&temp_search);
	system_dns_search_set_search(&temp_search, false);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			// set domain
			error = system_dns_search_set_domain(&temp_search, node_value);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_search_set_domain() error (%d)", error);
				goto error_out;
			}

			// add to the list
			error = system_dns_search_list_add(&ctx->temp_search_head, temp_search);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_search_list_add() error (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_MODIFIED:
			// not supported - cannot modify leaf-list element, it can be only created and deleted
			break;
		case SR_OP_DELETED:
			error = system_dns_search_list_remove(&ctx->temp_search_head, node_value);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_search_list_remove() error (%d)", error);
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
	system_dns_search_free(&temp_search);
	return error;
}

int system_dns_resolver_change_server_name(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
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

int system_dns_resolver_change_server_address(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	system_ctx_t *ctx = priv;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);
	system_dns_server_t temp_server = {0};
	system_ip_address_t temp_addr = {0};
	system_dns_server_element_t *found_server_el = NULL;

	system_dns_server_init(&temp_server);

	assert(strcmp(node_name, "address") == 0);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	// parse address to internal struct
	error = system_ip_address_from_str(&temp_addr, node_value);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ip_address_from_str() error (%d)", error);
		goto error_out;
	}

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			// set name to address
			error = system_dns_server_set_name(&temp_server, node_value);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_server_set_name() error (%d)", error);
				goto error_out;
			}

			// set address
			error = system_dns_server_set_address(&temp_server, temp_addr);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_server_set_address() error (%d)", error);
				goto error_out;
			}

			// add to the list
			error = system_dns_server_list_add(&ctx->temp_server_head, temp_server);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_server_list_add() error (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_MODIFIED:
			// get existing and modify
			found_server_el = system_dns_server_list_find(ctx->temp_server_head, change_ctx->previous_value);
			if (!found_server_el) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_server_list_find() error (%d)", error);
				goto error_out;
			}

			// modify name
			error = system_dns_server_set_name(&found_server_el->server, node_value);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_server_set_name() error (%d)", error);
				goto error_out;
			}

			// modify address
			error = system_dns_server_set_address(&found_server_el->server, temp_addr);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_server_set_address() error (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_DELETED:
			// remove element from the list
			error = system_dns_server_list_remove(&ctx->temp_server_head, node_value);
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
	system_dns_server_free(&temp_server);

	return error;
}

int system_dns_resolver_change_server_port(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
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