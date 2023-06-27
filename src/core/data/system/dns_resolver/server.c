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
#include "server.h"
#include <stdlib.h>
#include <string.h>

void system_dns_server_init(system_dns_server_t *server)
{
	*server = (system_dns_server_t){0};
}

int system_dns_server_set_name(system_dns_server_t *server, const char *name)
{
	int error = 0;

	if (server->name) {
		free((void *) server->name);
	}

	server->name = strdup(name);

	return error;
}

int system_dns_server_set_address(system_dns_server_t *server, system_ip_address_t address)
{
	int error = 0;
#ifdef SYSTEMD
	server->address = address;
#else
	if (server->address.value) {
		free((void *) server->address.value);
	}
	server->address.value = strdup(address.value);
#endif

	return error;
}

int system_dns_server_set_port(system_dns_server_t *server, int port)
{
	int error = 0;

	server->port = port;

	return error;
}

void system_dns_server_free(system_dns_server_t *server)
{
	if (server->name) {
		free((void *) server->name);
	}

#ifndef SYSTEMD
	if (server->address.value) {
		free((void *) server->address.value);
	}
#endif

	system_dns_server_init(server);
}
