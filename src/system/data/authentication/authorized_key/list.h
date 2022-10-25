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
#ifndef SYSTEM_PLUGIN_DATA_AUTHENTICATION_AUTHORIZED_KEY_LIST_H
#define SYSTEM_PLUGIN_DATA_AUTHENTICATION_AUTHORIZED_KEY_LIST_H

#include "plugin/types.h"

void system_authorized_key_list_init(system_authorized_key_element_t **head);
int system_authorized_key_list_add(system_authorized_key_element_t **head, system_authorized_key_t key);
system_authorized_key_element_t *system_authorized_key_list_find(system_authorized_key_element_t *head, const char *name);
int system_authorized_key_list_remove(system_authorized_key_element_t **head, const char *name);
int system_authorized_key_element_cmp_fn(void *e1, void *e2);
void system_authorized_key_list_free(system_authorized_key_element_t **head);

#endif // SYSTEM_PLUGIN_DATA_AUTHENTICATION_AUTHORIZED_KEY_LIST_H