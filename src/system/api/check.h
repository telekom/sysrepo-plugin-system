#ifndef SYSTEM_PLUGIN_API_CHECK_H
#define SYSTEM_PLUGIN_API_CHECK_H

#include "common.h"
#include "context.h"

#include <srpc.h>

srpc_check_status_t system_check_hostname(system_ctx_t *ctx, const char *hostname);
srpc_check_status_t system_check_contact(system_ctx_t *ctx, const char *contact);
srpc_check_status_t system_check_location(system_ctx_t *ctx, const char *location);
srpc_check_status_t system_check_timezone_name(system_ctx_t *ctx, const char *timezone_name);

#endif // SYSTEM_PLUGIN_API_CHECK_H