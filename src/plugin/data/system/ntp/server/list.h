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
#ifndef SYSTEM_PLUGIN_DATA_NTP_SERVER_LIST_H
#define SYSTEM_PLUGIN_DATA_NTP_SERVER_LIST_H

#include "plugin/types.h"

void system_ntp_server_list_init(system_ntp_server_element_t **head);
int system_ntp_server_list_add(system_ntp_server_element_t **head, system_ntp_server_t server);
system_ntp_server_element_t *system_ntp_server_list_find(system_ntp_server_element_t *head, const char *name);
int system_ntp_server_list_remove(system_ntp_server_element_t **head, const char *name);
int system_ntp_server_element_cmp_fn(void *e1, void *e2);
int system_ntp_server_element_address_cmp_fn(void *e1, void *e2);
void system_ntp_server_list_free(system_ntp_server_element_t **head);

#endif // SYSTEM_PLUGIN_DATA_NTP_SERVER_LIST_H