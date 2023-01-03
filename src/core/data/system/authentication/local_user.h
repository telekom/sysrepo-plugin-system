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
#ifndef SYSTEM_PLUGIN_DATA_AUTHENTICATION_LOCAL_USER_H
#define SYSTEM_PLUGIN_DATA_AUTHENTICATION_LOCAL_USER_H

#include "core/types.h"

void system_local_user_init(system_local_user_t *user);
int system_local_user_set_name(system_local_user_t *user, const char *name);
int system_local_user_set_password(system_local_user_t *user, const char *password);
void system_local_user_free(system_local_user_t *user);

int system_local_user_cmp_fn(const void *e1, const void *e2);

#endif // SYSTEM_PLUGIN_DATA_AUTHENTICATION_LOCAL_USER_H