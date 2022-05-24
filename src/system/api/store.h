#ifndef SYSTEM_PLUGIN_API_STORE_H
#define SYSTEM_PLUGIN_API_STORE_H

#include "context.h"

int system_store_hostname(system_ctx_t *ctx, const char *hostname);
int system_store_contact(system_ctx_t *ctx, const char *contact);
int system_store_location(system_ctx_t *ctx, const char *location);
int system_store_timezone_name(system_ctx_t *ctx, const char *timezone_name);

#endif // SYSTEM_PLUGIN_API_STORE_H