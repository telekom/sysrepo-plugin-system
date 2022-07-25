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
#ifndef SYSTEM_PLUGIN_API_AUTHENTICATION_STORE_H
#define SYSTEM_PLUGIN_API_AUTHENTICATION_STORE_H

#include "context.h"

int system_authentication_store_user(system_ctx_t *ctx, system_local_user_element_t *head);
int system_authentication_store_user_authorized_key(system_ctx_t *ctx, const char *user, system_authorized_key_element_t *head);

#endif // SYSTEM_PLUGIN_API_AUTHENTICATION_STORE_H