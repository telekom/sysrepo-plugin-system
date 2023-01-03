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
#ifndef SYSTEM_PLUGIN_API_DNS_RESOLVER_CHECK_H
#define SYSTEM_PLUGIN_API_DNS_RESOLVER_CHECK_H

#include "core/context.h"
#include "core/types.h"

#include <srpc.h>

srpc_check_status_t system_dns_resolver_check_search(system_ctx_t *ctx, system_dns_search_element_t *head);
srpc_check_status_t system_dns_resolver_check_server(system_ctx_t *ctx, system_dns_server_element_t *head);

#endif // SYSTEM_PLUGIN_API_DNS_RESOLVER_CHECK_H