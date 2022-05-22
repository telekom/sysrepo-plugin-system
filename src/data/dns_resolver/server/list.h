#ifndef SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SERVER_LIST_H
#define SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SERVER_LIST_H

#include "types.h"

void system_dns_server_list_init(system_dns_server_element_t **head);
int system_dns_server_list_add(system_dns_server_element_t **head, system_dns_server_t server);
void system_dns_server_list_free(system_dns_server_element_t **head);

#endif // SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SERVER_LIST_H