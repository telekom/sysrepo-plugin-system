#ifndef SYSTEM_PLUGIN_API_NTP_STORE_H
#define SYSTEM_PLUGIN_API_NTP_STORE_H

#include "types.h"
#include "context.h"

int system_ntp_store_server(system_ctx_t *ctx, const system_ntp_server_element_t *head);

#endif // SYSTEM_PLUGIN_API_NTP_STORE_H