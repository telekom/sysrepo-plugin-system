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
#ifndef SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SEARCH_H
#define SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SEARCH_H

#include "plugin/types.h"

void system_dns_search_init(system_dns_search_t *search);
int system_dns_search_set_domain(system_dns_search_t *search, const char *domain);
int system_dns_search_set_ifindex(system_dns_search_t *search, int ifindex);
int system_dns_search_set_search(system_dns_search_t *search, int s);
void system_dns_search_free(system_dns_search_t *search);

#endif // SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SEARCH_H