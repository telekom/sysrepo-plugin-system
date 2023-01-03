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
#ifndef SYSTEM_PLUGIN_API_CHECK_H
#define SYSTEM_PLUGIN_API_CHECK_H

#include "core/common.h"
#include "core/context.h"

#include <srpc.h>

srpc_check_status_t system_check_hostname(system_ctx_t *ctx, const char *hostname);
srpc_check_status_t system_check_contact(system_ctx_t *ctx, const char *contact);
srpc_check_status_t system_check_location(system_ctx_t *ctx, const char *location);
srpc_check_status_t system_check_timezone_name(system_ctx_t *ctx, const char *timezone_name);

#endif // SYSTEM_PLUGIN_API_CHECK_H