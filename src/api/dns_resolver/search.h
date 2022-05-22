#ifndef SYSTEM_PLUGIN_API_DNS_RESOLVER_SEARCH_H
#define SYSTEM_PLUGIN_API_DNS_RESOLVER_SEARCH_H

#include "types.h"

// load and store
int system_dns_resolver_load_search_values(system_dns_search_element_t **head);
int system_dns_resolver_store_search_values(system_dns_search_element_t *head);
void system_dns_resolver_free_search_values(system_dns_search_element_t **head);

// change
int system_dns_resolver_create_dns_search(const char *value);
int system_dns_resolver_modify_dns_search(const char *prev_value, const char *new_value);
int system_dns_resolver_delete_dns_search(const char *value);

#endif // SYSTEM_PLUGIN_API_DNS_RESOLVER_SEARCH_H