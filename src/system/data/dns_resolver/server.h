#ifndef SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SERVER_H
#define SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SERVER_H

#include "types.h"

void system_dns_server_init(system_dns_server_t *server);
int system_dns_server_set_name(system_dns_server_t *server, const char *name);
int system_dns_server_set_address(system_dns_server_t *server, system_ip_address_t address);
int system_dns_server_set_port(system_dns_server_t *server, int port);
void system_dns_server_free(system_dns_server_t *server);

#endif // SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SERVER_H