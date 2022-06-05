#ifndef SYSTEM_PLUGIN_API_CHANGE_H
#define SYSTEM_PLUGIN_API_CHANGE_H

#include "context.h"

#include <srpc.h>

int system_change_contact(system_ctx_t *ctx, srpc_change_node_t *change_node);
int system_change_hostname(system_ctx_t *ctx, srpc_change_node_t *change_node);
int system_change_location(system_ctx_t *ctx, srpc_change_node_t *change_node);
int system_change_timezone_name(system_ctx_t *ctx, srpc_change_node_t *change_node);

int system_change_contact_create(system_ctx_t *ctx, const char *value);
int system_change_contact_modify(system_ctx_t *ctx, const char *old_value, const char *new_value);
int system_change_contact_delete(system_ctx_t *ctx);

int system_change_location_create(system_ctx_t *ctx, const char *value);
int system_change_location_modify(system_ctx_t *ctx, const char *old_value, const char *new_value);
int system_change_location_delete(system_ctx_t *ctx);

int system_change_timezone_name_create(system_ctx_t *ctx, const char *value);
int system_change_timezone_name_modify(system_ctx_t *ctx, const char *old_value, const char *new_value);
int system_change_timezone_name_delete(system_ctx_t *ctx);

#endif // SYSTEM_PLUGIN_API_CHANGE_H