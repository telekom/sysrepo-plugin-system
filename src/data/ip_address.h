#ifndef SYSTEM_PLUGIN_DATA_IP_ADDRESS_H
#define SYSTEM_PLUGIN_DATA_IP_ADDRESS_H

#include "types.h"

void system_ip_address_init(system_ip_address_t *address);
int system_ip_address_to_str(system_ip_address_t *address, char *buffer, const unsigned int buffer_size);
void system_ip_address_free(system_ip_address_t *address);

#endif // SYSTEM_PLUGIN_DATA_NTP_SERVER_H