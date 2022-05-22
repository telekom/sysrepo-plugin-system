#ifndef SYSTEM_PLUGIN_SUBSCRIPTION_RPC_H
#define SYSTEM_PLUGIN_SUBSCRIPTION_RPC_H

#include <sysrepo_types.h>

// set-current-datetime //
int system_rpc_set_current_datetime(sr_session_ctx_t *session, uint32_t subscription_id, const char *op_path, const sr_val_t *input, const size_t input_cnt, sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data);

// restart //
int system_rpc_restart(sr_session_ctx_t *session, uint32_t subscription_id, const char *op_path, const sr_val_t *input, const size_t input_cnt, sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data);

// shutdown //
int system_rpc_shutdown(sr_session_ctx_t *session, uint32_t subscription_id, const char *op_path, const sr_val_t *input, const size_t input_cnt, sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data);

#endif // SYSTEM_PLUGIN_SUBSCRIPTION_RPC_H