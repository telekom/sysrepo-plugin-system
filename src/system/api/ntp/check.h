#ifndef SYSTEM_PLUGIN_API_NTP_CHECK_H
#define SYSTEM_PLUGIN_API_NTP_CHECK_H

#include "types.h"
#include "context.h"

#include <srpc.h>

srpc_check_status_t system_ntp_check_server(system_ctx_t *ctx, const system_ntp_server_element_t *head);

#endif // SYSTEM_PLUGIN_API_NTP_CHECK_H