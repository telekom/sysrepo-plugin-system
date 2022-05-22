#ifndef SYSTEM_PLUGIN_DNS_RESOLVER_H
#define SYSTEM_PLUGIN_DNS_RESOLVER_H

#include "common.h"

// search leaf-list
int system_dns_resolver_load_search_values(system_dns_search_element_t **head);
int system_dns_resolver_apply_search_values(system_dns_search_element_t *head);
void system_dns_resolver_free_search_values(system_dns_search_element_t **head);
int system_dns_resolver_create_dns_search(const char *value);
int system_dns_resolver_modify_dns_search(const char *prev_value, const char *new_value);
int system_dns_resolver_delete_dns_search(const char *value);

// server list
int system_dns_resolver_load_server_values(system_dns_server_element_t **head);
int system_dns_resolver_apply_server_values(system_dns_server_element_t *head);
void system_dns_resolver_free_server_values(system_dns_server_element_t **head);
int system_dns_resolver_create_dns_server_address(const char *value);
int system_dns_resolver_modify_dns_server_address(const char *prev_value, const char *new_value);
int system_dns_resolver_delete_dns_server_address(const char *value);
int system_dns_resolver_server_address_to_str(system_dns_server_t *server, char *buffer, unsigned int buffer_size);

#endif // SYSTEM_PLUGIN_DNS_RESOLVER_H