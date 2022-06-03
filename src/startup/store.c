#include "store.h"
#include "common.h"
#include "libyang/printer_data.h"
#include "ly_tree.h"

// API for getting system data
#include "srpc/types.h"
#include "system/api/check.h"
#include "system/api/store.h"
#include "system/api/dns_resolver/store.h"
#include "system/api/dns_resolver/check.h"
#include "system/api/authentication/store.h"

// data manipulation
#include "system/data/ip_address.h"
#include "system/data/dns_resolver/search.h"
#include "system/data/dns_resolver/search/list.h"
#include "system/data/dns_resolver/server.h"
#include "system/data/dns_resolver/server/list.h"
#include "types.h"

#include <sysrepo.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <sysrepo.h>
#include <libyang/libyang.h>
#include <libyang/tree.h>
#include <libyang/tree_schema.h>
#include <libyang/tree_data.h>

#include <srpc.h>

#include <utlist.h>

static int system_startup_store_hostname(void *priv, const struct lyd_node *system_container_node);
static int system_startup_store_contact(void *priv, const struct lyd_node *system_container_node);
static int system_startup_store_location(void *priv, const struct lyd_node *system_container_node);
static int system_startup_store_timezone_name(void *priv, const struct lyd_node *system_container_node);
static int system_startup_store_dns_resolver(void *priv, const struct lyd_node *system_container_node);
static int system_startup_store_authentication(void *priv, const struct lyd_node *system_container_node);

int system_startup_store_data(system_ctx_t *ctx, sr_session_ctx_t *session)
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
			system_startup_store_hostname,
		},
		{
			"contact",
			system_startup_store_contact,
		},
		{
			"location",
			system_startup_store_location,
		},
		{
			"timezone-name",
			system_startup_store_timezone_name,
		},
		{
			"dns-resolver",
			system_startup_store_dns_resolver,
		},
		{
			"authentication",
			system_startup_store_authentication,
		},
	};

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

static int system_startup_store_hostname(void *priv, const struct lyd_node *system_container_node)
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

static int system_startup_store_contact(void *priv, const struct lyd_node *system_container_node)
{
	int error = 0;
	system_ctx_t *ctx = (system_ctx_t *) priv;

	struct lyd_node *contact_node = srpc_ly_tree_get_child_leaf(system_container_node, "contact");

	if (contact_node) {
		const char *contact = lyd_get_value(contact_node);

		SRPLG_LOG_INF(PLUGIN_NAME, "contact value: %s", contact);

		error = system_store_contact(ctx, contact);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_store_contact() failed (%d)", error);
			return -1;
		}
	}

	return 0;
}

static int system_startup_store_location(void *priv, const struct lyd_node *system_container_node)
{
	int error = 0;
	system_ctx_t *ctx = (system_ctx_t *) priv;

	struct lyd_node *location_node = srpc_ly_tree_get_child_leaf(system_container_node, "location");

	if (location_node) {
		const char *location = lyd_get_value(location_node);

		SRPLG_LOG_INF(PLUGIN_NAME, "location value: %s", location);

		error = system_store_location(ctx, location);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_store_location() failed (%d)", error);
			return -1;
		}
	}

	return 0;
}

static int system_startup_store_timezone_name(void *priv, const struct lyd_node *system_container_node)
{
	int error = 0;
	system_ctx_t *ctx = (system_ctx_t *) priv;
	srpc_check_status_t check_status = srpc_check_status_none;

	struct lyd_node *clock_container_node = NULL, *timezone_name_node = NULL;

	clock_container_node = srpc_ly_tree_get_child_container(system_container_node, "clock");

	if (clock_container_node) {
		timezone_name_node = srpc_ly_tree_get_child_leaf(system_container_node, "timezone-name");
		if (timezone_name_node) {
			const char *timezone_name = lyd_get_value(timezone_name_node);

			SRPLG_LOG_INF(PLUGIN_NAME, "Checking system timezone-name value");
			check_status = system_check_timezone_name(ctx, timezone_name);

			switch (check_status) {
				case srpc_check_status_none:
					SRPLG_LOG_ERR(PLUGIN_NAME, "Error loading current system timezone-name value");
					goto error_out;
					break;
				case srpc_check_status_error:
					SRPLG_LOG_ERR(PLUGIN_NAME, "Error loading current system timezone-name value");
					goto error_out;
					break;
				case srpc_check_status_non_existant:
					SRPLG_LOG_INF(PLUGIN_NAME, "Storing timezone-name value %s", timezone_name);

					error = system_store_timezone_name(ctx, timezone_name);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "system_store_timezone_name() failed (%d)", error);
						return -1;
					}
					break;
				case srpc_check_status_equal:
					SRPLG_LOG_ERR(PLUGIN_NAME, "Startup timezone-name value is already applied on the system");
					break;
				case srpc_check_status_partial:
					// should not be returned - treat as error
					SRPLG_LOG_ERR(PLUGIN_NAME, "Error loading current system timezone-name value");
					goto error_out;
					break;
			}
		}
	}

	goto out;

error_out:
	error = -1;

out:
	return 0;
}

static int system_startup_store_dns_resolver(void *priv, const struct lyd_node *system_container_node)
{
	int error = 0;
	system_ctx_t *ctx = (system_ctx_t *) priv;
	struct lyd_node *dns_resolver_container_node = NULL;
	struct lyd_node *search_leaf_list_node = NULL;
	struct lyd_node *server_list_node = NULL;
	struct lyd_node *server_name_leaf_node = NULL;
	struct lyd_node *server_address_leaf_node = NULL;
	struct lyd_node *server_port_leaf_node = NULL;
	struct lyd_node *udp_and_tcp_container_node = NULL;
	system_dns_server_element_t *servers_head = NULL;
	system_dns_search_element_t *search_head = NULL;
	system_dns_server_t tmp_server = {0};
	system_dns_search_t tmp_search = {0};
	system_ip_address_t tmp_ip = {0};
	srpc_check_status_t search_check_status = srpc_check_status_none, server_check_status = srpc_check_status_none;

	SRPLG_LOG_INF(PLUGIN_NAME, "Loading dns-resolver startup data");

	dns_resolver_container_node = srpc_ly_tree_get_child_container(system_container_node, "dns-resolver");
	if (dns_resolver_container_node) {
		// get search leaf list
		search_leaf_list_node = srpc_ly_tree_get_child_leaf_list(dns_resolver_container_node, "search");
		server_list_node = srpc_ly_tree_get_child_list(dns_resolver_container_node, "server");

		if (search_leaf_list_node) {
			// store search list
			while (search_leaf_list_node) {
				system_dns_search_init(&tmp_search);

				const char *domain = lyd_get_value(search_leaf_list_node);

				SRPLG_LOG_INF(PLUGIN_NAME, "Adding DNS search value %s", domain);

				error = system_dns_search_set_domain(&tmp_search, domain);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_search_set_domain() error (%d)", error);
					goto error_out;
				}

				error = system_dns_search_set_ifindex(&tmp_search, 0);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_search_set_ifindex() error (%d)", error);
					goto error_out;
				}

				error = system_dns_search_set_search(&tmp_search, 0);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_search_set_search() error (%d)", error);
					goto error_out;
				}

				// add to the list
				error = system_dns_search_list_add(&search_head, tmp_search);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_search_list_add() error (%d)", error);
					goto error_out;
				}

				system_dns_search_free(&tmp_search);

				search_leaf_list_node = srpc_ly_tree_get_leaf_list_next(search_leaf_list_node);
			}

			SRPLG_LOG_INF(PLUGIN_NAME, "Checking DNS search values on the system");
			search_check_status = system_dns_resolver_check_search(ctx, search_head);
			SRPLG_LOG_INF(PLUGIN_NAME, "Recieved check status = %d", search_check_status);

			switch (search_check_status) {
				case srpc_check_status_none:
					// should not be recieved - treat same as an error
					SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured while checking DNS search system values");
					goto error_out;
					break;
				case srpc_check_status_error:
					SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured while checking DNS search system values");
					goto error_out;
					break;
				case srpc_check_status_non_existant:
					// values don't exist - apply them to the system
					SRPLG_LOG_INF(PLUGIN_NAME, "Storing DNS search values from the datastore to the system");

					// apply search values to the system
					error = system_dns_resolver_store_search(ctx, search_head);
					if (error) {
						SRPLG_LOG_INF(PLUGIN_NAME, "system_dns_resolver_store_search() error (%d)", error);
						goto error_out;
					}

					SRPLG_LOG_INF(PLUGIN_NAME, "Stored DNS search values from the datastore to the system");
					break;
				case srpc_check_status_equal:
					// values exist - don't do anything
					break;
				case srpc_check_status_partial:
					// TODO: create an union between startup and system values and apply that list to the system
					break;
			}
		}

		if (server_list_node) {
			while (server_list_node) {
				// process server list node
				system_dns_server_init(&tmp_server);

				server_name_leaf_node = srpc_ly_tree_get_child_leaf(server_list_node, "name");
				udp_and_tcp_container_node = srpc_ly_tree_get_child_container(server_list_node, "udp-and-tcp");

				const char *name = lyd_get_value(server_name_leaf_node);
				SRPLG_LOG_INF(PLUGIN_NAME, "Adding DNS server %s", name);

				if (!udp_and_tcp_container_node) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_ly_tree_get_child_container() failed for udp-and-tcp");
					goto error_out;
				}

				server_address_leaf_node = srpc_ly_tree_get_child_leaf(udp_and_tcp_container_node, "address");
				server_port_leaf_node = srpc_ly_tree_get_child_leaf(udp_and_tcp_container_node, "port");

				// address
				if (server_address_leaf_node) {
					const char *address = lyd_get_value(server_address_leaf_node);

					// parse address
					error = system_ip_address_from_str(&tmp_ip, address);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "system_ip_address_from_str() error (%d)", error);
						goto error_out;
					}

					// set address
					error = system_dns_server_set_address(&tmp_server, tmp_ip);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_server_set_address() error (%d)", error);
						goto error_out;
					}
				} else {
					// no address node -> unable to continue
					SRPLG_LOG_INF(PLUGIN_NAME, "srpc_ly_tree_get_child_leaf() failed for leaf address");
					goto error_out;
				}

				if (server_port_leaf_node) {
					const char *port = lyd_get_value(server_port_leaf_node);

					// parse into int
					const int port_i32 = atoi(port);

					// set port
					error = system_dns_server_set_port(&tmp_server, port_i32);
					if (error) {
						SRPLG_LOG_INF(PLUGIN_NAME, "system_dns_server_set_port() error (%d)", error);
						goto error_out;
					}
				}

				// append to the list
				error = system_dns_server_list_add(&servers_head, tmp_server);
				if (error) {
					SRPLG_LOG_INF(PLUGIN_NAME, "system_dns_server_list_add() error (%d)", error);
					goto error_out;
				}

				system_dns_server_free(&tmp_server);

				server_list_node = srpc_ly_tree_get_list_next(server_list_node);
			}

			SRPLG_LOG_INF(PLUGIN_NAME, "Checking DNS server values on the system");
			server_check_status = system_dns_resolver_check_server(ctx, servers_head);
			SRPLG_LOG_INF(PLUGIN_NAME, "Recieved check status = %d", server_check_status);

			switch (server_check_status) {
				case srpc_check_status_none:
					// should not be recieved - treat same as an error
					SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured while checking DNS server system values");
					goto error_out;
					break;
				case srpc_check_status_error:
					SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured while checking DNS server system values");
					goto error_out;
					break;
				case srpc_check_status_non_existant:
					SRPLG_LOG_INF(PLUGIN_NAME, "Storing DNS server values from the datastore to the system");

					// gathered all servers - store them to the system
					error = system_dns_resolver_store_server(ctx, servers_head);
					if (error) {
						SRPLG_LOG_INF(PLUGIN_NAME, "system_dns_resolver_store_server() error (%d)", error);
						goto error_out;
					}

					SRPLG_LOG_INF(PLUGIN_NAME, "Stored DNS server values from the datastore to the system");
					break;
				case srpc_check_status_equal:
					// don't do anything
					SRPLG_LOG_INF(PLUGIN_NAME, "DNS server values already exist on the system - no need to apply anyhing");
					break;
				case srpc_check_status_partial:
					// TODO: implement
					break;
			}
		}
	}

	goto out;

error_out:
	error = -1;

out:
	// remove temp values if something was interrupted
	system_dns_search_free(&tmp_search);
	system_dns_server_free(&tmp_server);

	// free allocated lists
	system_dns_search_list_free(&search_head);
	system_dns_server_list_free(&servers_head);

	return error;
}

static int system_startup_store_authentication(void *priv, const struct lyd_node *system_container_node)
{
	int error = 0;

	system_ctx_t *ctx = (system_ctx_t *) priv;
	struct lyd_node *authentication_container_node = NULL;

	SRPLG_LOG_INF(PLUGIN_NAME, "Storing authentication startup data");

	authentication_container_node = srpc_ly_tree_get_child_container(system_container_node, "dns-resolver");
	if (authentication_container_node) {
	}

	return error;
}