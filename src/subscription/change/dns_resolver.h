#ifndef SYSTEM_PLUGIN_SUBSCRIPTION_CHANGE_DNS_RESOLVER_H
#define SYSTEM_PLUGIN_SUBSCRIPTION_CHANGE_DNS_RESOLVER_H

// search leaf-list
int system_create_dns_search(const char *value);
int system_modify_dns_search(const char *prev_value, const char *new_value);
int system_delete_dns_search(const char *value);

// server/address
int system_create_dns_server_address(const char *value);
int system_modify_dns_server_address(const char *prev_value, const char *new_value);
int system_delete_dns_server_address(const char *value);

#endif // SYSTEM_PLUGIN_SUBSCRIPTION_CHANGE_DNS_RESOLVER_H