#ifndef SYSTEM_PLUGIN_API_DNS_RESOLVER_CHECK_H
#define SYSTEM_PLUGIN_API_DNS_RESOLVER_CHECK_H

#include "context.h"
#include "types.h"

#include <srpc.h>

srpc_check_status_t system_dns_resolver_check_search(system_ctx_t *ctx, system_dns_search_element_t *head);
srpc_check_status_t system_dns_resolver_check_server(system_ctx_t *ctx, system_dns_server_element_t *head);

#endif // SYSTEM_PLUGIN_API_DNS_RESOLVER_CHECK_H