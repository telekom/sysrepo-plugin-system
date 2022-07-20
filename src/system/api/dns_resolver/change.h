#ifndef SYSTEM_PLUGIN_API_DNS_RESOLVER_CHANGE_H
#define SYSTEM_PLUGIN_API_DNS_RESOLVER_CHANGE_H

#include "context.h"

#include <srpc.h>

int system_dns_resolver_change_search(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx);

int system_dns_resolver_change_server_name(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx);
int system_dns_resolver_change_server_address(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx);
int system_dns_resolver_change_server_port(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx);

#endif // SYSTEM_PLUGIN_API_DNS_RESOLVER_CHANGE_H