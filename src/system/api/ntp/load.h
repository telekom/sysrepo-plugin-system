#ifndef SYSTEM_PLUGIN_API_NTP_LOAD_H
#define SYSTEM_PLUGIN_API_NTP_LOAD_H

#include "types.h"
#include "context.h"

int system_ntp_load_server(system_ctx_t *ctx, system_ntp_server_element_t **head);

#endif // SYSTEM_PLUGIN_API_NTP_LOAD_H