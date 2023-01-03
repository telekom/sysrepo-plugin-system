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
#ifndef SYSTEM_PLUGIN_API_DNS_RESOLVER_STORE_H
#define SYSTEM_PLUGIN_API_DNS_RESOLVER_STORE_H

#include "core/types.h"
#include "core/context.h"

int system_dns_resolver_store_search(system_ctx_t *ctx, system_dns_search_element_t *head);
int system_dns_resolver_store_server(system_ctx_t *ctx, system_dns_server_element_t *head);

#endif // SYSTEM_PLUGIN_API_DNS_RESOLVER_STORE_H