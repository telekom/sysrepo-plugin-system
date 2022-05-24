#ifndef SYSTEM_PLUGIN_AUTHENTICATION_DATA_AUTHORIZED_KEY_ARRAY_H
#define SYSTEM_PLUGIN_AUTHENTICATION_DATA_AUTHORIZED_KEY_ARRAY_H

#include "types.h"

void system_authorized_key_array_init(UT_array **keys);
void system_authorized_key_array_free(UT_array **keys);

#endif // SYSTEM_PLUGIN_AUTHENTICATION_DATA_AUTHORIZED_KEY_ARRAY_H