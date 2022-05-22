#ifndef SYSTEM_PLUGIN_API_NTP_SERVER_H
#define SYSTEM_PLUGIN_API_NTP_SERVER_H

#include <utarray.h>

int system_ntp_load_server_values(UT_array **servers);
int system_ntp_store_server_values(UT_array *servers);

#endif // SYSTEM_PLUGIN_API_NTP_SERVER_H