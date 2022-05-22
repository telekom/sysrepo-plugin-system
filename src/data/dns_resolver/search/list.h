#ifndef SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SEARCH_LIST_H
#define SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SEARCH_LIST_H

#include "types.h"

void system_dns_search_list_init(system_dns_search_element_t **head);
void system_dns_search_list_free(system_dns_search_element_t **head);

#endif // SYSTEM_PLUGIN_DATA_DNS_RESOLVER_SEARCH_LIST_H