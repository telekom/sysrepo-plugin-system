#ifndef SYSTEM_PLUGIN_API_DNS_RESOLVER_SERVER_H
#define SYSTEM_PLUGIN_API_DNS_RESOLVER_SERVER_H

#include "types.h"

// load and store
int system_dns_resolver_load_server_values(system_dns_server_element_t **head);
int system_dns_resolver_store_server_values(system_dns_server_element_t *head);
void system_dns_resolver_free_server_values(system_dns_server_element_t **head);

// change
int system_dns_resolver_create_dns_server_address(const char *value);
int system_dns_resolver_modify_dns_server_address(const char *prev_value, const char *new_value);
int system_dns_resolver_delete_dns_server_address(const char *value);

#endif // SYSTEM_PLUGIN_API_DNS_RESOLVER_SERVER_H