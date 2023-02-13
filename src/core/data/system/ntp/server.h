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
#ifndef SYSTEM_PLUGIN_DATA_NTP_SERVER_H
#define SYSTEM_PLUGIN_DATA_NTP_SERVER_H

#include "core/types.h"

void system_ntp_server_init(system_ntp_server_t *server);
int system_ntp_server_set_name(system_ntp_server_t *server, const char *name);
int system_ntp_server_set_address(system_ntp_server_t *server, const char *address);
int system_ntp_server_set_port(system_ntp_server_t *server, const char *port);
int system_ntp_server_set_association_type(system_ntp_server_t *server, const char *association_type);
int system_ntp_server_set_iburst(system_ntp_server_t *server, const char *iburst);
int system_ntp_server_set_prefer(system_ntp_server_t *server, const char *prefer);
void system_ntp_server_free(system_ntp_server_t *server);

#endif // SYSTEM_PLUGIN_DATA_NTP_SERVER_H