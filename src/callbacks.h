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

#ifndef GENERAL_PLUGIN_CALLBACKS_H
#define GENERAL_PLUGIN_CALLBACKS_H

#include <sysrepo.h>
#include "context.h"
#include "utils/ntp/server_list.h"

int system_module_change_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);
int system_state_data_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);
int system_rpc_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *op_path, const sr_val_t *input, const size_t input_cnt, sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data);

int system_set_config_value(system_ctx_t *ctx, const char *xpath, const char *value, sr_change_oper_t operation);
int system_set_ntp(system_ctx_t *ctx, const char *xpath, char *value);
int system_set_dns(system_ctx_t *ctx, const char *xpath, char *value, sr_change_oper_t operation);
int system_set_user_authentication(system_ctx_t *ctx, const char *xpath, char *value);
#ifndef SYSTEMD
static int set_dns_timeout(char *value);
static int set_dns_attempts(char *value);
#endif
int system_set_contact_info(const char *value);
int system_set_timezone(const char *value);
int system_set_datetime(char *datetime);
int system_set_location(const char *location);

int system_ntp_set_server_name(char *name, char *address);
int system_ntp_get_server_name(char **name, char *address);
int system_ntp_set_entry_datastore(sr_session_ctx_t *session, ntp_server_t *server_entry);

#endif /* GENERAL_PLUGIN_CALLBACKS_H */