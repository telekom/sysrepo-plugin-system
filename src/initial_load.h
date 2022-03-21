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

#ifndef GENERAL_PLUGIN_INITIAL_LOAD_H
#define GENERAL_PLUGIN_INITIAL_LOAD_H

// project
#include "context.h"

// lib
#include <sysrepo.h>

int system_initial_load(system_ctx_t *ctx, sr_session_ctx_t *session);

#endif // GENERAL_PLUGIN_INITIAL_LOAD_H