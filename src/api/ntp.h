#ifndef SYSTEM_PLUGIN_NTP_H
#define SYSTEM_PLUGIN_NTP_H

#include <utarray.h>

int system_ntp_load_server_values(UT_array **servers);
int system_ntp_apply_server_values(UT_array *servers);

#endif // SYSTEM_PLUGIN_NTP_H