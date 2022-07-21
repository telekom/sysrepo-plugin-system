#include "load.h"
#include "common.h"
#include "context.h"
#include "ly_tree.h"

// API for getting system data
#include "srpc/common.h"
#include "srpc/ly_tree.h"
#include "system/api/load.h"
#include "system/api/authentication/load.h"
#include "system/api/dns_resolver/load.h"

// data manipulation
#include "system/api/ntp/load.h"
#include "system/data/authentication/authorized_key/list.h"
#include "system/data/authentication/local_user/list.h"
#include "system/data/ip_address.h"
#include "system/data/dns_resolver/search/list.h"
#include "system/data/dns_resolver/server/list.h"
#include "system/data/ntp/server/list.h"

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
	bool timezone_name_enabled = false;

	SRPC_SAFE_CALL(srpc_check_feature_status(ctx->startup_session, BASE_YANG_MODEL, "timezone-name", &timezone_name_enabled), error_out);

	if (timezone_name_enabled) {
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

	system_ctx_t *ctx = priv;

	// ietf-system nodes
	struct lyd_node *ntp_container_node = NULL, *server_list_node = NULL;

	// feature check
	bool ntp_enabled = false;
	bool ntp_udp_port_enabled = false;

	// load list
	system_ntp_server_element_t *ntp_server_head = NULL, *ntp_server_iter = NULL;

	SRPLG_LOG_INF(PLUGIN_NAME, "Loading NTP data");

	SRPC_SAFE_CALL(srpc_check_feature_status(ctx->startup_session, BASE_YANG_MODEL, "ntp", &ntp_enabled), error_out);
	SRPC_SAFE_CALL(srpc_check_feature_status(ctx->startup_session, BASE_YANG_MODEL, "ntp-udp-port", &ntp_udp_port_enabled), error_out);

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
	struct lyd_node *user_list_node = NULL, *authorized_key_list_node = NULL;
	system_local_user_element_t *user_head = NULL, *user_iter = NULL;
	system_authorized_key_element_t *key_iter = NULL;

	bool enabled_authentication = false;
	bool enabled_local_users = false;

	SRPC_SAFE_CALL(srpc_check_feature_status(ctx->startup_session, BASE_YANG_MODEL, "authentication", &enabled_authentication), error_out);
	SRPC_SAFE_CALL(srpc_check_feature_status(ctx->startup_session, BASE_YANG_MODEL, "local-users", &enabled_local_users), error_out);

	if (enabled_authentication) {
		// create authentication container
		error = system_ly_tree_create_authentication(ly_ctx, parent_node, &authentication_container_node);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_authentication() error (%d)", error);
			goto error_out;
		}

		if (enabled_local_users) {
			SRPLG_LOG_INF(PLUGIN_NAME, "Loading users from the system");

			// init list first
			system_local_user_list_init(&user_head);

			// load user list
			error = system_authentication_load_user(ctx, &user_head);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_load_user() error (%d)", error);
				goto error_out;
			}

			SRPLG_LOG_INF(PLUGIN_NAME, "Loading user authorized keys");

			LL_FOREACH(user_head, user_iter)
			{
				system_authorized_key_list_init(&user_iter->user.key_head);

				error = system_authentication_load_user_authorized_key(ctx, user_iter->user.name, &user_iter->user.key_head);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_load_user_authorized_key() error (%d)", error);
					goto error_out;
				}
			}

			SRPLG_LOG_INF(PLUGIN_NAME, "Saving users and their keys to the datastore");

			LL_FOREACH(user_head, user_iter)
			{
				// list item
				error = system_ly_tree_create_authentication_user(ly_ctx, authentication_container_node, &user_list_node, user_iter->user.name);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_authentication_user() error (%d) for %s", error, user_iter->user.name);
					goto error_out;
				}

				// password
				if (user_iter->user.password) {
					error = system_ly_tree_create_authentication_user_password(ly_ctx, user_list_node, user_iter->user.password);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_authentication_user_password() error (%d) for %s", error, user_iter->user.password);
						goto error_out;
					}
				}

				// authorized-key
				int count = 0;
				LL_COUNT(user_iter->user.key_head, key_iter, count);

				if (count > 0) {
					key_iter = NULL;
					LL_FOREACH(user_iter->user.key_head, key_iter)
					{
						// list item
						error = system_ly_tree_create_authentication_user_authorized_key(ly_ctx, user_list_node, &authorized_key_list_node, key_iter->key.name);
						if (error) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_authentication_user_authorized_key() error (%d) for %s", error, key_iter->key.name);
							goto error_out;
						}

						// algorithm
						if (key_iter->key.algorithm) {
							error = system_ly_tree_create_authentication_user_authorized_key_algorithm(ly_ctx, authorized_key_list_node, key_iter->key.algorithm);
							if (error) {
								SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_authentication_user_authorized_key_algorithm() error (%d) for %s", error, key_iter->key.algorithm);
								goto error_out;
							}
						}

						// key-data
						if (key_iter->key.data) {
							error = system_ly_tree_create_authentication_user_authorized_key_data(ly_ctx, authorized_key_list_node, key_iter->key.data);
							if (error) {
								SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_authentication_user_authorized_key_data() error (%d) for %s", error, key_iter->key.data);
								goto error_out;
							}
						}
					}
				}
			}

			SRPLG_LOG_INF(PLUGIN_NAME, "Saved users to the datastore");
		}
	}

	goto out;

error_out:
	error = -1;

out:
	system_local_user_list_free(&user_head);

	return error;
}