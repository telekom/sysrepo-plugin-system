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
#ifndef SYSTEM_PLUGIN_API_AUTHENTICATION_LOAD_H
#define SYSTEM_PLUGIN_API_AUTHENTICATION_LOAD_H

#include "plugin/context.h"
#include "plugin/types.h"

int system_authentication_load_user(system_ctx_t *ctx, system_local_user_element_t **head);
int system_authentication_load_user_authorized_key(system_ctx_t *ctx, const char *user, system_authorized_key_element_t **head);

#endif // SYSTEM_PLUGIN_API_AUTHENTICATION_LOAD_H