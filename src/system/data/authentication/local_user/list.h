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
#ifndef SYSTEM_PLUGIN_DATA_AUTHENTICATION_LOCAL_USER_LIST_H
#define SYSTEM_PLUGIN_DATA_AUTHENTICATION_LOCAL_USER_LIST_H

#include "types.h"

void system_local_user_list_init(system_local_user_element_t **head);
int system_local_user_list_add(system_local_user_element_t **head, system_local_user_t user);
system_local_user_element_t *system_local_user_list_find(system_local_user_element_t *head, const char *name);
system_local_user_element_t *system_local_user_list_complement(system_local_user_element_t *union_head, system_local_user_element_t *head);
int system_local_user_list_remove(system_local_user_element_t **head, const char *name);
int system_local_user_element_cmp_fn(void *e1, void *e2);
void system_local_user_list_free(system_local_user_element_t **head);

#endif // SYSTEM_PLUGIN_DATA_AUTHENTICATION_LOCAL_USER_LIST_H