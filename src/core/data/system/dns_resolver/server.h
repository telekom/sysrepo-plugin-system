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
#ifndef SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SERVER_H
#define SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SERVER_H

#include "core/types.h"

void system_dns_server_init(system_dns_server_t *server);
int system_dns_server_set_name(system_dns_server_t *server, const char *name);
int system_dns_server_set_address(system_dns_server_t *server, system_ip_address_t address);
int system_dns_server_set_port(system_dns_server_t *server, int port);
void system_dns_server_free(system_dns_server_t *server);

#endif // SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SERVER_H