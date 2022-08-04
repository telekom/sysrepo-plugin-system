/*
 * telekom / sysrepo-plugin-system
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2022 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "store.h"
#include "common.h"
#include "libyang/printer_data.h"
#include "ly_tree.h"

// API for getting system data
#include "srpc/common.h"
#include "srpc/ly_tree.h"
#include "srpc/types.h"
#include "system/api/authentication/check.h"
#include "system/api/authentication/load.h"
#include "system/api/check.h"
#include "system/api/ntp/check.h"
#include "system/api/ntp/store.h"
#include "system/api/store.h"
#include "system/api/dns_resolver/store.h"
#include "system/api/dns_resolver/check.h"
#include "system/api/authentication/store.h"

// data manipulation
#include "system/data/authentication/authorized_key.h"
#include "system/data/authentication/authorized_key/list.h"
#include "system/data/authentication/local_user.h"
#include "system/data/authentication/local_user/list.h"
#include "system/data/ip_address.h"
#include "system/data/dns_resolver/search.h"
#include "system/data/dns_resolver/search/list.h"
#include "system/data/dns_resolver/server.h"
#include "system/data/dns_resolver/server/list.h"
#include "system/data/ntp/server.h"
#include "system/data/ntp/server/list.h"
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
static int system_startup_store_ntp(void *priv, const struct lyd_node *system_container_node);
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
			"ntp",
			system_startup_store_ntp,
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
	bool timezone_name_enabled = false;

	struct lyd_node *clock_container_node = NULL, *timezone_name_node = NULL;

	SRPC_SAFE_CALL(srpc_check_feature_status(ctx->startup_session, BASE_YANG_MODULE, "timezone-name", &timezone_name_enabled), error_out);

	if (timezone_name_enabled) {
		clock_container_node = srpc_ly_tree_get_child_container(system_container_node, "clock");

		if (clock_container_node) {
			timezone_name_node = srpc_ly_tree_get_child_leaf(clock_container_node, "timezone-name");
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
	}

	goto out;

error_out:
	error = -1;

out:
	return 0;
}

static int system_startup_store_ntp(void *priv, const struct lyd_node *system_container_node)
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

	bool ntp_enabled = false;
	bool ntp_udp_port_enabled = false;

	system_ntp_server_t temp_server = {0};
	srpc_check_status_t server_check_status = srpc_check_status_none;

	SRPC_SAFE_CALL(srpc_check_feature_status(ctx->startup_session, BASE_YANG_MODULE, "ntp", &ntp_enabled), error_out);
	SRPC_SAFE_CALL(srpc_check_feature_status(ctx->startup_session, BASE_YANG_MODULE, "ntp-udp-port", &ntp_udp_port_enabled), error_out);

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

	SRPLG_LOG_INF(PLUGIN_NAME, "Storing dns-resolver startup data");

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

				// SRPLG_LOG_INF(PLUGIN_NAME, "Adding DNS search value %s", domain);

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
				// SRPLG_LOG_INF(PLUGIN_NAME, "Adding DNS server %s", name);

				// set name
				system_dns_server_set_name(&tmp_server, name);

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
					SRPLG_LOG_INF(PLUGIN_NAME, "DNS server values already exist on the system - no need to apply anything");
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

	// libyang
	struct lyd_node *authentication_container_node = NULL;
	struct lyd_node *local_user_list_node = NULL, *user_name_leaf_node = NULL, *user_password_leaf_node;
	struct lyd_node *authorized_key_list_node = NULL, *key_name_leaf_node = NULL, *key_algorithm_leaf_node = NULL, *key_data_leaf_node = NULL;

	// data
	system_local_user_element_t *user_head = NULL, *found_user_el = NULL, *user_iter = NULL;
	system_local_user_element_t *system_user_head = NULL;
	system_local_user_element_t *complement_user_head = NULL;
	system_local_user_t temp_user = {0};
	system_authorized_key_t temp_key = {0};

	// features
	bool authentication_enabled = false;
	bool local_users_enabled = false;

	// srpc
	srpc_check_status_t user_check_status = srpc_check_status_none, key_check_status = srpc_check_status_none;

	SRPC_SAFE_CALL(srpc_check_feature_status(ctx->startup_session, BASE_YANG_MODULE, "authentication", &authentication_enabled), error_out);
	SRPC_SAFE_CALL(srpc_check_feature_status(ctx->startup_session, BASE_YANG_MODULE, "local-users", &local_users_enabled), error_out);

	if (authentication_enabled) {
		SRPLG_LOG_INF(PLUGIN_NAME, "Storing authentication startup data");

		authentication_container_node = srpc_ly_tree_get_child_container(system_container_node, "authentication");
		if (authentication_container_node) {
			if (local_users_enabled) {
				SRPLG_LOG_INF(PLUGIN_NAME, "Storing local-users startup data");

				local_user_list_node = srpc_ly_tree_get_child_list(authentication_container_node, "user");
				while (local_user_list_node) {
					// create user
					system_local_user_init(&temp_user);

					user_name_leaf_node = srpc_ly_tree_get_child_leaf(local_user_list_node, "name");
					user_password_leaf_node = srpc_ly_tree_get_child_leaf(local_user_list_node, "password");

					// a name has to exist for the list element
					assert(user_name_leaf_node != NULL);

					// set name
					error = system_local_user_set_name(&temp_user, lyd_get_value(user_name_leaf_node));
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_set_name() error (%d)", error);
						goto error_out;
					}

					// set password
					if (user_password_leaf_node) {
						error = system_local_user_set_password(&temp_user, lyd_get_value(user_password_leaf_node));
						if (error) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_set_password() error (%d)", error);
							goto error_out;
						}
					}

					// add user to the list
					error = system_local_user_list_add(&user_head, temp_user);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_add() error (%d)", error);
						goto error_out;
					}

					// get current user
					found_user_el = system_local_user_list_find(user_head, temp_user.name);
					if (!found_user_el) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_find() failed");
						goto error_out;
					}

					// user found - setup keys if any
					authorized_key_list_node = srpc_ly_tree_get_child_list(local_user_list_node, "authorized-key");
					while (authorized_key_list_node) {
						// add key to the current user (found_user_el)
						system_authorized_key_init(&temp_key);

						key_name_leaf_node = srpc_ly_tree_get_child_leaf(authorized_key_list_node, "name");
						key_algorithm_leaf_node = srpc_ly_tree_get_child_leaf(authorized_key_list_node, "algorithm");
						key_data_leaf_node = srpc_ly_tree_get_child_leaf(authorized_key_list_node, "key-data");

						// key must have a name
						assert(key_name_leaf_node != NULL);

						error = system_authorized_key_set_name(&temp_key, lyd_get_value(key_name_leaf_node));
						if (error) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_set_name() error (%d)", error);
							goto error_out;
						}

						if (key_algorithm_leaf_node) {
							// set algorithm
							error = system_authorized_key_set_algorithm(&temp_key, lyd_get_value(key_algorithm_leaf_node));
							if (error) {
								SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_set_algorithm() error (%d)", error);
								goto error_out;
							}
						}

						if (key_data_leaf_node) {
							// set algorithm
							error = system_authorized_key_set_data(&temp_key, lyd_get_value(key_data_leaf_node));
							if (error) {
								SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_set_data() error (%d)", error);
								goto error_out;
							}
						}

						// add to the list of current user keys
						error = system_authorized_key_list_add(&found_user_el->user.key_head, temp_key);
						if (error) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_list_add() error (%d)", error);
							goto error_out;
						}

						// free temp data
						system_authorized_key_free(&temp_key);
						authorized_key_list_node = srpc_ly_tree_get_list_next(authorized_key_list_node);
					}

					system_local_user_free(&temp_user);
					local_user_list_node = srpc_ly_tree_get_list_next(local_user_list_node);
				}

				// get system users
				system_local_user_list_init(&system_user_head);

				// check if all users exist on the system
				SRPLG_LOG_INF(PLUGIN_NAME, "Checking startup local user system values");
				user_check_status = system_authentication_check_user(ctx, user_head, &system_user_head);
				SRPLG_LOG_INF(PLUGIN_NAME, "Recieved local users check status: %d", user_check_status);

				switch (user_check_status) {
					case srpc_check_status_none:
						SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured while checking local user system values");
						goto error_out;
						break;
					case srpc_check_status_error:
						SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured while checking local user system values");
						goto error_out;
						break;
					case srpc_check_status_non_existant:
						SRPLG_LOG_INF(PLUGIN_NAME, "Startup local users don\'t exist on the system - starting to store startup local users");
						error = system_authentication_store_user(ctx, user_head);
						if (error) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_store_user() error (%d)", error);
							goto error_out;
						}
						break;
					case srpc_check_status_equal:
						SRPLG_LOG_INF(PLUGIN_NAME, "Startup local users already exist on the system - no need to store anything");
						break;
					case srpc_check_status_partial:
						SRPLG_LOG_INF(PLUGIN_NAME, "Some startup local users exist while others don\'t - creating non existant users");

						// get complement of startup without system
						complement_user_head = system_local_user_list_complement(user_head, system_user_head);
						if (!complement_user_head) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_complement() failed");
							goto error_out;
						}

						SRPLG_LOG_INF(PLUGIN_NAME, "Storing missing local users from startup to the system");

						// add complement users to system
						error = system_authentication_store_user(ctx, complement_user_head);
						if (error) {
							SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_store_user() error (%d)", error);
							goto error_out;
						}

						SRPLG_LOG_INF(PLUGIN_NAME, "Missing local users from startup are stored in the system");
						break;
				}

				// after matching startup and system values for users - match key lists for all users
				SRPLG_LOG_INF(PLUGIN_NAME, "Checking startup local user authorized key system values");
				LL_FOREACH(user_head, user_iter)
				{
					// check first if any keys exist in startup
					if (user_iter->user.key_head) {
						key_check_status = system_authentication_check_user_authorized_key(ctx, user_iter->user.name, user_iter->user.key_head);
						SRPLG_LOG_INF(PLUGIN_NAME, "Recieved authorized-key check status %d for user %s", user_check_status, user_iter->user.name);

						switch (key_check_status) {
							case srpc_check_status_none:
								SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured while checking local user authorized key system values");
								goto error_out;
								break;
							case srpc_check_status_error:
								SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured while checking local user authorized key system values");
								goto error_out;
								break;
							case srpc_check_status_non_existant:
								SRPLG_LOG_INF(PLUGIN_NAME, "Startup authorized keys don\'t exist on the system for user %s - storing authorized keys for user %s", user_iter->user.name, user_iter->user.name);
								error = system_authentication_store_user_authorized_key(ctx, user_iter->user.name, user_iter->user.key_head);
								if (error) {
									SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_store_user_authorized_key() error (%d) for user %s", error, user_iter->user.name);
									goto error_out;
								}
								break;
							case srpc_check_status_equal:
								SRPLG_LOG_INF(PLUGIN_NAME, "Startup authorized keys already exist for local user %s - no need to store anything", user_iter->user.name);
								break;
							case srpc_check_status_partial:
								// TODO
								break;
						}
					}
				}
			}
		}
	}

	goto out;

error_out:
	error = -1;

out:

	// free temp values if interrupted
	system_local_user_free(&temp_user);
	system_authorized_key_free(&temp_key);

	// free lists
	system_local_user_list_free(&user_head);
	system_local_user_list_free(&system_user_head);
	system_local_user_list_free(&complement_user_head);

	return error;
}