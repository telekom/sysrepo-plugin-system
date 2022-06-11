#ifndef SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SERVER_LIST_H
#define SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SERVER_LIST_H

#include "types.h"

void system_dns_server_list_init(system_dns_server_element_t **head);
int system_dns_server_list_add(system_dns_server_element_t **head, system_dns_server_t server);
system_dns_server_element_t *system_dns_server_list_find(system_dns_server_element_t *head, const char *name);
int system_dns_server_list_remove(system_dns_server_element_t **head, const char *name);
int system_dns_server_element_cmp_fn(void *e1, void *e2);
void system_dns_server_list_free(system_dns_server_element_t **head);

#endif // SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SERVER_LIST_H