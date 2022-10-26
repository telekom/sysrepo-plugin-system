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
#ifndef SYSTEM_PLUGIN_API_STORE_H
#define SYSTEM_PLUGIN_API_STORE_H

#include "plugin/context.h"

int system_store_hostname(system_ctx_t *ctx, const char *hostname);
int system_store_contact(system_ctx_t *ctx, const char *contact);
int system_store_location(system_ctx_t *ctx, const char *location);
int system_store_timezone_name(system_ctx_t *ctx, const char *timezone_name);

#endif // SYSTEM_PLUGIN_API_STORE_H