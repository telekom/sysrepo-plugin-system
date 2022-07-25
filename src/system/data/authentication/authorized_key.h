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
#ifndef SYSTEM_PLUGIN_DATA_AUTHENTICATION_AUTHORIZED_KEY_H
#define SYSTEM_PLUGIN_DATA_AUTHENTICATION_AUTHORIZED_KEY_H

#include "types.h"

void system_authorized_key_init(system_authorized_key_t *key);
int system_authorized_key_set_name(system_authorized_key_t *key, const char *name);
int system_authorized_key_set_algorithm(system_authorized_key_t *key, const char *algorithm);
int system_authorized_key_set_data(system_authorized_key_t *key, const char *data);
void system_authorized_key_free(system_authorized_key_t *key);

int system_authorized_key_cmp_fn(const void *e1, const void *e2);

#endif // SYSTEM_PLUGIN_DATA_AUTHENTICATION_AUTHORIZED_KEY_H