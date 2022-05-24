#include "load.h"
#include "common.h"
#include "context.h"
#include "ly_tree.h"

// API for getting system data
#include "system/api/load.h"
#include "system/authentication/api/load.h"
#include "system/authentication/data/local_user/array.h"
#include "system/dns_resolver/api/load.h"

// data manipulation
#include "system/data/ip_address.h"
#include "system/dns_resolver/data/search/list.h"
#include "system/dns_resolver/data/server/list.h"

#include <sysrepo.h>
#include <unistd.h>
#include <errno.h>

#include <sysrepo.h>

#include <srpc.h>

#include <utlist.h>

static int system_startup_load_hostname(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);
static int system_startup_load_contact(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);
static int system_startup_load_location(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);
static int system_startup_load_timezone_name(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);
static int system_startup_load_ntp(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);
static int system_startup_load_dns_resolver(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);
static int system_startup_load_authentication(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);

int system_startup_load_data(system_ctx_t *ctx, sr_session_ctx_t *session)
{
	int error = 0;

	const struct ly_ctx *ly_ctx = NULL;
	struct lyd_node *system_container_node = NULL;
	sr_conn_ctx_t *conn_ctx = NULL;

	srpc_startup_load_t load_values[] = {
		{
			"hostname",
			system_startup_load_hostname,
		},
		{
			"contact",
			system_startup_load_contact,
		},
		{
			"location",
			system_startup_load_location,
		},
		{
			"timezone-name",
			system_startup_load_timezone_name,
		},
		{
			"ntp",
			system_startup_load_ntp,
		},
		{
			"dns-resolver",
			system_startup_load_dns_resolver,
		},
		{
			"authentication",
			system_startup_load_authentication,
		},
	};

	conn_ctx = sr_session_get_connection(session);
	ly_ctx = sr_acquire_context(conn_ctx);
	if (ly_ctx == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to get ly_ctx variable");
		goto error_out;
	}

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
	if (system_container_node) {
		lyd_free_tree(system_container_node);
	}
	sr_release_context(conn_ctx);
	return error;
}

static int system_startup_load_hostname(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node)
{
	int error = 0;
	system_ctx_t *ctx = (system_ctx_t *) priv;
	char hostname_buffer[SYSTEM_HOSTNAME_LENGTH_MAX] = {0};

	error = system_load_hostname(ctx, hostname_buffer);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_load_hostname() error (%d)", error);
		goto error_out;
	}

	error = system_ly_tree_create_hostname(ly_ctx, parent_node, hostname_buffer);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_hostname() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	return error;
}

static int system_startup_load_contact(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node)
{
	int error = 0;
	return error;
}

static int system_startup_load_location(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node)
{
	int error = 0;
	return error;
}

static int system_startup_load_timezone_name(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node)
{
	int error = 0;
	system_ctx_t *ctx = (system_ctx_t *) priv;
	char timezone_name_buffer[SYSTEM_TIMEZONE_NAME_LENGTH_MAX] = {0};
	struct lyd_node *clock_container_node = NULL;

	error = system_load_timezone_name(ctx, timezone_name_buffer);

	// setup clock container
	error = system_ly_tree_create_clock(ly_ctx, parent_node, &clock_container_node);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_clock_container() error (%d)", error);
		goto error_out;
	}

	// set timezone-name leaf
	error = system_ly_tree_create_timezone_name(ly_ctx, clock_container_node, timezone_name_buffer);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_timezone_name() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	return error;
}

static int system_startup_load_ntp(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node)
{
	int error = 0;
	return error;
}

static int system_startup_load_dns_resolver(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node)
{
	int error = 0;
	system_ctx_t *ctx = (system_ctx_t *) priv;
	struct lyd_node *dns_resolver_container_node = NULL, *server_list_node = NULL;
	system_dns_search_element_t *search_head = NULL, *search_iter_el = NULL;
	system_dns_server_element_t *servers_head = NULL, *servers_iter_el = NULL;
	char address_buffer[100] = {0};
	char port_buffer[10] = {0};

	// setup dns-resolver container
	error = system_ly_tree_create_dns_resolver(ly_ctx, parent_node, &dns_resolver_container_node);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_dns_resolver() error (%d)", error);
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Loading DNS search values from the system");

	// load values

	error = system_dns_resolver_load_search(ctx, &search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_load_search_values() error (%d)", error);
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Loading DNS server values from the system");

	error = system_dns_resolver_load_server(ctx, &servers_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_load_server_values() error (%d)", error);
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Saving search values to the datastore");

	LL_FOREACH(search_head, search_iter_el)
	{
		error = system_ly_tree_append_dns_resolver_search(ly_ctx, dns_resolver_container_node, search_iter_el->search.domain);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_append_dns_resolver_search() error (%d) for %s", error, search_iter_el->search.domain);
			goto error_out;
		}
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Saved search values to the datastore");
	SRPLG_LOG_INF(PLUGIN_NAME, "Saving server values to the datastore");

	LL_FOREACH(servers_head, servers_iter_el)
	{
		error = system_ip_address_to_str(&servers_iter_el->server.address, address_buffer, sizeof(address_buffer));
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_server_address_to_str() error (%d)", error);
			goto error_out;
		}

		error = system_ly_tree_create_dns_resolver_server(ly_ctx, dns_resolver_container_node, &server_list_node, address_buffer);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_dns_resolver_server() error (%d) for %s", error, address_buffer);
			goto error_out;
		}

		// address
		error = system_ly_tree_create_dns_resolver_server_address(ly_ctx, server_list_node, address_buffer);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_dns_resolver_server_address() error (%d) for %s", error, address_buffer);
			goto error_out;
		}

		// port
		if (servers_iter_el->server.port != 0) {
			error = snprintf(port_buffer, sizeof(port_buffer), "%d", servers_iter_el->server.port);
			if (error < 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error (%d) for %d port", error, servers_iter_el->server.port);
				goto error_out;
			}

			error = system_ly_tree_create_dns_resolver_server_port(ly_ctx, server_list_node, port_buffer);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_dns_resolver_server_address() error (%d) for %s port", error, port_buffer);
				goto error_out;
			}
		}
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Saved server values to the datastore");

	goto out;

error_out:
	error = -1;

out:
	// free values
	system_dns_search_list_free(&search_head);
	system_dns_server_list_free(&servers_head);

	return error;
}

static int system_startup_load_authentication(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node)
{
	int error = 0;
	system_ctx_t *ctx = (system_ctx_t *) priv;
	struct lyd_node *authentication_container_node = NULL;
	struct lyd_node *user_list_node = NULL;
	UT_array *users = NULL;
	system_local_user_t *user_iter = NULL;

	// create authentication container
	error = system_ly_tree_create_authentication(ly_ctx, parent_node, &authentication_container_node);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_authentication() error (%d)", error);
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Loading users from the system");

	// init array first
	system_local_user_array_init(&users);

	// load user list
	error = system_authentication_load_user(ctx, &users);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_load_user() error (%d)", error);
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Saving users to the datastore");

	while ((user_iter = utarray_next(users, user_iter)) != NULL) {
		// list item
		error = system_ly_tree_create_authentication_user(ly_ctx, authentication_container_node, &user_list_node, user_iter->name);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_authentication_user() error (%d) for %s", error, user_iter->name);
			goto error_out;
		}

		// password
		if (user_iter->password) {
			error = system_ly_tree_create_authentication_user_password(ly_ctx, user_list_node, user_iter->password);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_authentication_user_password() error (%d) for %s", error, user_iter->password);
				goto error_out;
			}
		}
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Saved users to the datastore");

	goto out;

error_out:
	error = -1;

out:
	system_local_user_array_free(&users);

	return error;
}