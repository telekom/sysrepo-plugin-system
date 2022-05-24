#ifndef SYSTEM_PLUGIN_DNS_RESOLVER_API_STORE_H
#define SYSTEM_PLUGIN_DNS_RESOLVER_API_STORE_H

#include "types.h"
#include "context.h"

int system_dns_resolver_store_search(system_ctx_t *ctx, system_dns_search_element_t *head);
int system_dns_resolver_store_server(system_ctx_t *ctx, system_dns_server_element_t *head);

#endif // SYSTEM_PLUGIN_DNS_RESOLVER_API_STORE_H