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
#ifndef SYSTEM_PLUGIN_DATA_IP_ADDRESS_H
#define SYSTEM_PLUGIN_DATA_IP_ADDRESS_H

#include "plugin/types.h"

void system_ip_address_init(system_ip_address_t *address);
int system_ip_address_to_str(system_ip_address_t *address, char *buffer, const unsigned int buffer_size);
int system_ip_address_from_str(system_ip_address_t *address, const char *str);
void system_ip_address_free(system_ip_address_t *address);

#endif // SYSTEM_PLUGIN_DATA_NTP_SERVER_H