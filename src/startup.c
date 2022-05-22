#include "startup.h"
#include "common.h"
#include "dns_resolver.h"
#include "ly_tree.h"
#include "srpc/types.h"

#include <sysrepo.h>
#include <unistd.h>
#include <errno.h>

#include <srpc.h>

#include <utlist.h>

// helpers

static int system_startup_load_hostname(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);
static int system_startup_load_contact(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);
static int system_startup_load_location(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);
static int system_startup_load_timezone_name(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);
static int system_startup_load_ntp(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);
static int system_startup_load_dns_resolver(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);
static int system_startup_load_authentication(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);

////

int system_startup_load_data(system_ctx_t *ctx, sr_session_ctx_t *session)
{
	int error = 0;

	const struct ly_ctx *ly_ctx = NULL;
	struct lyd_node *system_container_node = NULL;
	sr_conn_ctx_t *conn_ctx = NULL;

	srpc_startup_load_cb system_container_callbacks[] = {
		system_startup_load_hostname,
		system_startup_load_contact,
		system_startup_load_location,
		system_startup_load_timezone_name,
		system_startup_load_ntp,
		system_startup_load_dns_resolver,
		system_startup_load_authentication,
	};

	conn_ctx = sr_session_get_connection(session);
	ly_ctx = sr_acquire_context(conn_ctx);
	if (ly_ctx == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to get ly_ctx variable");
		goto error_out;
	}

	// load system container info
	error = system_ly_tree_create_system(ly_ctx, &system_container_node);
	for (size_t i = 0; i < ARRAY_SIZE(system_container_callbacks); i++) {
		srpc_startup_load_cb cb = system_container_callbacks[i];

		error = cb((void *) ctx, session, ly_ctx, system_container_node);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "Node creation callback failed");
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

int system_startup_apply_data(system_ctx_t *ctx, sr_session_ctx_t *session)
{
	int error = 0;
	return error;
}

static int system_startup_load_hostname(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node)
{
	int error = 0;
	char hostname_buffer[SYSTEM_HOSTNAME_LENGTH_MAX] = {0};

	error = gethostname(hostname_buffer, sizeof(hostname_buffer));
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "gethostname() error: %s", strerror(errno));
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
	char timezone_name_buffer[PATH_MAX] = {0};
	char timezone_path_buffer[PATH_MAX] = {0};
	struct lyd_node *clock_container_node = NULL;

	ssize_t len = 0;
	size_t start = 0;

	len = readlink(SYSTEM_LOCALTIME_FILE, timezone_path_buffer, sizeof(timezone_path_buffer) - 1);
	if (len == -1) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "readlink() error");
		goto error_out;
	}

	// terminate path
	timezone_path_buffer[len] = 0;

	// assert start is equal to the timezone dir path
	if (strncmp(timezone_path_buffer, SYSTEM_TIMEZONE_DIR, sizeof(SYSTEM_TIMEZONE_DIR) - 1) != 0) {
		goto error_out;
	}

	// fetch the rest of the path into timezone_name_buffer
	start = sizeof(SYSTEM_TIMEZONE_DIR);
	strcpy(timezone_name_buffer, &timezone_path_buffer[start]);

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

	// load values

	error = system_dns_resolver_load_search_values(&search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_load_search_values() error (%d)", error);
		goto error_out;
	}

	error = system_dns_resolver_load_server_values(&servers_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_load_server_values() error (%d)", error);
		goto error_out;
	}

	LL_FOREACH(search_head, search_iter_el)
	{
		error = system_ly_tree_append_dns_resolver_search(ly_ctx, dns_resolver_container_node, search_iter_el->search.domain);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_append_dns_resolver_search() error (%d) for %s", error, search_iter_el->search.domain);
			goto error_out;
		}
	}

	LL_FOREACH(servers_head, servers_iter_el)
	{
		error = system_dns_resolver_server_address_to_str(&servers_iter_el->server, address_buffer, sizeof(address_buffer));
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

	goto out;

error_out:
	error = -1;

out:
	// free values
	system_dns_resolver_free_search_values(&search_head);
	system_dns_resolver_free_server_values(&servers_head);

	return error;
}

static int system_startup_load_authentication(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node)
{
	int error = 0;
	return error;
}