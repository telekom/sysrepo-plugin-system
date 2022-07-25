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
#ifndef SYSTEM_PLUGIN_API_LOAD_H
#define SYSTEM_PLUGIN_API_LOAD_H

#include "common.h"
#include "context.h"

int system_load_hostname(system_ctx_t *ctx, char buffer[SYSTEM_HOSTNAME_LENGTH_MAX]);
int system_load_contact(system_ctx_t *ctx, char buffer[256]);
int system_load_location(system_ctx_t *ctx, char buffer[256]);
int system_load_timezone_name(system_ctx_t *ctx, char buffer[SYSTEM_TIMEZONE_NAME_LENGTH_MAX]);

#endif // SYSTEM_PLUGIN_API_LOAD_H