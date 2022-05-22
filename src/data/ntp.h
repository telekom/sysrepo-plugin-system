#ifndef SYSTEM_PLUGIN_DATA_NTP_H
#define SYSTEM_PLUGIN_DATA_NTP_H

#include "types.h"

#include <utarray.h>

// server
void system_ntp_server_init(system_ntp_server_t *server);
int system_ntp_server_set_name(system_ntp_server_t *server, const char *name);
int system_ntp_server_set_address(system_ntp_server_t *server, const char *address);
int system_ntp_server_set_port(system_ntp_server_t *server, int port);
int system_ntp_server_set_association_type(system_ntp_server_t *server, const char *association_type);
int system_ntp_server_set_iburst(system_ntp_server_t *server, const char *iburst);
int system_ntp_server_set_prefer(system_ntp_server_t *server, const char *prefer);
void system_ntp_server_free(system_ntp_server_t *server);

// server array
void system_ntp_server_array_init(UT_array **servers);
void system_ntp_server_array_free(UT_array **servers);

#endif // SYSTEM_PLUGIN_DATA_NTP_H