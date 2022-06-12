#ifndef SYSTEM_PLUGIN_DATA_NTP_SERVER_LIST_H
#define SYSTEM_PLUGIN_DATA_NTP_SERVER_LIST_H

#include "types.h"

void system_ntp_server_list_init(system_ntp_server_element_t **head);
int system_ntp_server_list_add(system_ntp_server_element_t **head, system_ntp_server_t server);
system_ntp_server_element_t *system_ntp_server_list_find(system_ntp_server_element_t *head, const char *name);
int system_ntp_server_list_remove(system_ntp_server_element_t **head, const char *name);
int system_ntp_server_element_cmp_fn(void *e1, void *e2);
int system_ntp_server_element_address_cmp_fn(void *e1, void *e2);
void system_ntp_server_list_free(system_ntp_server_element_t **head);

#endif // SYSTEM_PLUGIN_DATA_NTP_SERVER_LIST_H