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
#include "load.h"
#include "common.h"

// data
#include "system/data/dns_resolver/server.h"
#include "system/data/dns_resolver/server/list.h"
#include "system/data/dns_resolver/search/list.h"
#include "system/data/ip_address.h"
#include "utils/memory.h"

#include <systemd/sd-bus.h>

#include <sysrepo.h>

#include <utlist.h>

int system_dns_resolver_load_search(system_ctx_t *ctx, system_dns_search_element_t **head)
{
	int error = 0;
	system_dns_search_t tmp_search = {0};

#ifdef SYSTEMD
	int r;
	sd_bus_message *msg = NULL;
	sd_bus_error sdb_err = SD_BUS_ERROR_NULL;
	sd_bus *bus = NULL;

	r = sd_bus_open_system(&bus);
	if (r < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Failed to open system bus: %s\n", strerror(-r));
		goto invalid;
	}

	r = sd_bus_get_property(
		bus,
		"org.freedesktop.resolve1",
		"/org/freedesktop/resolve1",
		"org.freedesktop.resolve1.Manager",
		"Domains",
		&sdb_err,
		&msg,
		"a(isb)");

	if (r < 0) {
		goto invalid;
	}

	// message recieved -> enter msg and get needed info
	r = sd_bus_message_enter_container(msg, 'a', "(isb)");
	if (r < 0) {
		goto invalid;
	}

	for (;;) {
		r = sd_bus_message_enter_container(msg, 'r', "isb");
		if (r < 0) {
			goto invalid;
		}

		if (r == 0) {
			// done with reading data
			break;
		}

		// read Domain struct
		r = sd_bus_message_read(msg, "isb", &tmp_search.ifindex, &tmp_search.domain, &tmp_search.search);
		if (r < 0) {
			goto invalid;
		}

		// leave Domain struct
		r = sd_bus_message_exit_container(msg);
		if (r < 0) {
			goto invalid;
		}

		error = system_dns_search_list_add(head, tmp_search);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_search_list_add() error (%d)", error);
			goto invalid;
		}
	}

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

int system_dns_resolver_load_server(system_ctx_t *ctx, system_dns_server_element_t **head)
{
	int error = 0;

#ifdef SYSTEMD
	int r;
	sd_bus_message *msg = NULL;
	sd_bus_error sdb_err = SD_BUS_ERROR_NULL;
	sd_bus *bus = NULL;
	int tmp_ifindex = 0;
	size_t tmp_length = 0;

	char ip_buffer[46] = {0};

	system_dns_server_t tmp_server = {0};

	r = sd_bus_open_system(&bus);
	if (r < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Failed to open system bus: %s\n", strerror(-r));
		goto finish;
	}

	r = sd_bus_get_property(
		bus,
		"org.freedesktop.resolve1",
		"/org/freedesktop/resolve1",
		"org.freedesktop.resolve1.Manager",
		"DNS",
		&sdb_err,
		&msg,
		"a(iiay)");

	if (r < 0) {
		goto invalid;
	}

	r = sd_bus_message_enter_container(msg, 'a', "(iiay)");
	if (r < 0) {
		goto invalid;
	}

	for (;;) {
		const void *data;
		r = sd_bus_message_enter_container(msg, 'r', "iiay");
		if (r < 0) {
			goto invalid;
		}
		if (r == 0) {
			break;
		}
		r = sd_bus_message_read(msg, "ii", &tmp_ifindex, &tmp_server.address.family);
		if (r < 0) {
			goto invalid;
		}

		switch (tmp_server.address.family) {
			case AF_INET:
				r = sd_bus_message_read_array(msg, 'y', (const void **) &data, &tmp_length);
				if (r >= 0) {
					memcpy(tmp_server.address.value.v4, data, tmp_length);
				}
				break;
			case AF_INET6:
				r = sd_bus_message_read_array(msg, 'y', (const void **) &data, &tmp_length);
				if (r >= 0) {
					memcpy(tmp_server.address.value.v6, data, tmp_length);
				}
				break;
			default:
				// unknown address family -> for now abort
				r = -1;
				break;
		}

		if (r < 0) {
			goto invalid;
		}

		r = sd_bus_message_exit_container(msg);
		if (r < 0) {
			goto invalid;
		}

		// setup name to be equal to the address -> convert IP to string
		error = system_ip_address_to_str(&tmp_server.address, ip_buffer, sizeof(ip_buffer));
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_ip_address_to_str() error (%d)", error);
			goto invalid;
		}

		// copy to the current server name
		tmp_server.name = xstrdup(ip_buffer);

		error = system_dns_server_list_add(head, tmp_server);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_server_list_add() error (%d)", error);
			goto invalid;
		}

		// free temp server
		system_dns_server_free(&tmp_server);
	}

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