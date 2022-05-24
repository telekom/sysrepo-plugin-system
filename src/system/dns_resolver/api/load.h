#ifndef SYSTEM_PLUGIN_DNS_RESOLVER_API_LOAD_H
#define SYSTEM_PLUGIN_DNS_RESOLVER_API_LOAD_H

#include "types.h"
#include "context.h"

int system_dns_resolver_load_search(system_ctx_t *ctx, system_dns_search_element_t **head);
int system_dns_resolver_load_server(system_ctx_t *ctx, system_dns_server_element_t **head);

#endif // SYSTEM_PLUGIN_DNS_RESOLVER_API_LOAD_H