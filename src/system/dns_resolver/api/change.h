#ifndef SYSTEM_PLUGIN_DNS_RESOLVER_API_CHANGE_H
#define SYSTEM_PLUGIN_DNS_RESOLVER_API_CHANGE_H

#include "context.h"

int system_dns_resolver_change_search_create(system_ctx_t *ctx, const char *value);
int system_dns_resolver_change_search_modify(system_ctx_t *ctx, const char *prev_value, const char *new_value);
int system_dns_resolver_change_search_delete(system_ctx_t *ctx, const char *value);

int system_dns_resolver_change_server_create(system_ctx_t *ctx, const char *value);
int system_dns_resolver_change_server_modify(system_ctx_t *ctx, const char *prev_value, const char *new_value);
int system_dns_resolver_change_server_delete(system_ctx_t *ctx, const char *value);

#endif // SYSTEM_PLUGIN_DNS_RESOLVER_API_CHANGE_H