#ifndef SYSTEM_PLUGIN_AUTHENTICATION_DATA_AUTHORIZED_KEY_H
#define SYSTEM_PLUGIN_AUTHENTICATION_DATA_AUTHORIZED_KEY_H

#include "types.h"

void system_authorized_key_init(system_authorized_key_t *key);
int system_authorized_key_set_name(system_authorized_key_t *key, const char *name);
int system_authorized_key_set_algorithm(system_authorized_key_t *key, const char *algorithm);
int system_authorized_key_set_data(system_authorized_key_t *key, const char *data);
void system_authorized_key_free(system_authorized_key_t *key);

#endif // SYSTEM_PLUGIN_AUTHENTICATION_DATA_AUTHORIZED_KEY_H