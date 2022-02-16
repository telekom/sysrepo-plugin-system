/*
 * telekom / sysrepo-plugin-system
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2021 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef GENERAL_PLUGIN_CONTEXT_H
#define GENERAL_PLUGIN_CONTEXT_H

#include <sysrepo.h>

#include "sysrepo_types.h"
#include "utils/dns/server.h"
#include "utils/uthash/utarray.h"

typedef struct system_ctx_s {
	dns_server_element_t *dns_servers_head;
	UT_array *ntp_servers;
	UT_array *local_users;
	sr_session_ctx_t *startup_session;
} system_ctx_t;

#endif // GENERAL_PLUGIN_CONTEXT_H