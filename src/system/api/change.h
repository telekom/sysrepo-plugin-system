#ifndef SYSTEM_PLUGIN_API_CHANGE_H
#define SYSTEM_PLUGIN_API_CHANGE_H

#include "context.h"

#include <srpc.h>

int system_change_contact(system_ctx_t *ctx, const srpc_change_node_t *change_node);
int system_change_hostname(system_ctx_t *ctx, const srpc_change_node_t *change_node);
int system_change_location(system_ctx_t *ctx, const srpc_change_node_t *change_node);
int system_change_timezone_name(system_ctx_t *ctx, const srpc_change_node_t *change_node);

#endif // SYSTEM_PLUGIN_API_CHANGE_H