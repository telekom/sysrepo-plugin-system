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
#include "core/common.h"

#include <systemd/sd-bus.h>

#include <sysrepo.h>

#include <utlist.h>

int system_dns_resolver_store_search(system_ctx_t *ctx, system_dns_search_element_t *head)
{
	int error = 0;
	system_dns_search_element_t *search_iter_el = NULL;

#ifdef SYSTEMD
	int r;
	sd_bus_error sdb_err = SD_BUS_ERROR_NULL;
	sd_bus_message *msg = NULL;
	sd_bus_message *reply = NULL;
	sd_bus *bus = NULL;

	r = sd_bus_open_system(&bus);
	if (r < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Failed to open system bus: %s\n", strerror(-r));
		goto invalid;
	}

	r = sd_bus_message_new_method_call(
		bus,
		&msg,
		"org.freedesktop.resolve1",
		"/org/freedesktop/resolve1",
		"org.freedesktop.resolve1.Manager",
		"SetLinkDomains");
	if (r < 0) {
		goto invalid;
	}

	// set ifindex to the first value in the list
	r = sd_bus_message_append(msg, "i", SYSTEMD_IFINDEX);
	if (r < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sd_bus_message_append() error");
		goto invalid;
	}

	r = sd_bus_message_open_container(msg, 'a', "(sb)");
	if (r < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sd_bus_message_open_container() error");
		goto invalid;
	}

	LL_FOREACH(head, search_iter_el)
	{
		r = sd_bus_message_append(msg, "(sb)", search_iter_el->search.domain, search_iter_el->search.search);
		if (r < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sd_bus_message_append() error");
			goto invalid;
		}
	}

	r = sd_bus_message_close_container(msg);
	if (r < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sd_bus_message_close_container() error");
		goto invalid;
	}

	r = sd_bus_call(bus, msg, 0, &sdb_err, &reply);
	if (r < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sd_bus_call() error");
		goto invalid;
	}

	// SRP_LOG_INF("Set domains successfully!");
	goto finish;

invalid:
	SRPLG_LOG_ERR(PLUGIN_NAME, "sd-bus failure (%d): %s", r, sdb_err.message);
	error = -1;

finish:
	sd_bus_message_unref(msg);
	sd_bus_flush_close_unref(bus);
#else
#endif

	return error;
}

int system_dns_resolver_store_server(system_ctx_t *ctx, system_dns_server_element_t *head)
{
	int error = 0;
	system_dns_server_element_t *server_iter_el = NULL;

#ifdef SYSTEMD
	int r;
	sd_bus_error sdb_err = SD_BUS_ERROR_NULL;
	sd_bus_message *msg = NULL;
	sd_bus_message *reply = NULL;
	sd_bus *bus = NULL;

	r = sd_bus_open_system(&bus);
	if (r < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Failed to open system bus: %s\n", strerror(-r));
		goto invalid;
	}

	r = sd_bus_message_new_method_call(
		bus,
		&msg,
		"org.freedesktop.resolve1",
		"/org/freedesktop/resolve1",
		"org.freedesktop.resolve1.Manager",
		"SetLinkDNS");
	if (r < 0) {
		goto invalid;
	}

	r = sd_bus_message_append(msg, "i", SYSTEMD_IFINDEX);
	if (r < 0) {
		goto invalid;
	}

	// enter array of structs
	r = sd_bus_message_open_container(msg, 'a', "(iay)");
	if (r < 0) {
		goto invalid;
	}

	LL_FOREACH(head, server_iter_el)
	{
		system_dns_server_t *server = &server_iter_el->server;

		// enter a struct first
		r = sd_bus_message_open_container(msg, 'r', "iay");
		if (r < 0) {
			goto invalid;
		}

		// set address family
		r = sd_bus_message_append(msg, "i", server->address.family);
		if (r < 0) {
			goto invalid;
		}

		// enter array of bytes for an address
		r = sd_bus_message_open_container(msg, 'a', "y");
		if (r < 0) {
			goto invalid;
		}

		// append address bytes accordingly with address family
		switch (server->address.family) {
			case AF_INET:
				for (unsigned char j = 0; j < ARRAY_SIZE(server->address.value.v4); j++) {
					r = sd_bus_message_append(msg, "y", server->address.value.v4[j]);
					if (r < 0) {
						goto invalid;
					}
				}
				break;
			case AF_INET6:
				for (unsigned char j = 0; j < ARRAY_SIZE(server->address.value.v6); j++) {
					r = sd_bus_message_append(msg, "y", server->address.value.v6[j]);
					if (r < 0) {
						goto invalid;
					}
				}
				break;
			default:
				break;
		}

		// exit array
		r = sd_bus_message_close_container(msg);
		if (r < 0) {
			goto invalid;
		}

		// exit struct
		r = sd_bus_message_close_container(msg);
		if (r < 0) {
			goto invalid;
		}
	}

	// exit array of structs
	r = sd_bus_message_close_container(msg);
	if (r < 0) {
		goto invalid;
	}

	// finally call created method
	r = sd_bus_call(bus, msg, 0, &sdb_err, &reply);
	if (r < 0) {
		goto invalid;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Set DNS servers successfully.");
	goto finish;

invalid:
	SRPLG_LOG_ERR(PLUGIN_NAME, "sd-bus failure (%d): %s", r, sdb_err.message);
	error = -1;

finish:
	sd_bus_message_unref(msg);
	sd_bus_message_unref(reply);
	sd_bus_flush_close_unref(bus);
#else
#endif

	return error;
}
