#ifndef SYSTEM_PLUGIN_DATA_NTP_SERVER_ARRAY_H
#define SYSTEM_PLUGIN_DATA_NTP_SERVER_ARRAY_H

#include <utarray.h>

void system_ntp_server_array_init(UT_array **servers);
void system_ntp_server_array_free(UT_array **servers);

#endif // SYSTEM_PLUGIN_DATA_NTP_SERVER_ARRAY_H