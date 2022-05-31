#ifndef SYSTEM_PLUGIN_DATA_AUTHENTICATION_AUTHORIZED_KEY_ARRAY_H
#define SYSTEM_PLUGIN_DATA_AUTHENTICATION_AUTHORIZED_KEY_ARRAY_H

#include "types.h"

void system_authorized_key_array_init(UT_array **keys);
int system_authorized_key_array_add(UT_array **keys, system_authorized_key_t key);
void system_authorized_key_array_free(UT_array **keys);

#endif // SYSTEM_PLUGIN_DATA_AUTHENTICATION_AUTHORIZED_KEY_ARRAY_H