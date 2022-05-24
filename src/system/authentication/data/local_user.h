#ifndef SYSTEM_PLUGIN_AUTHENTICATION_DATA_LOCAL_USER_H
#define SYSTEM_PLUGIN_AUTHENTICATION_DATA_LOCAL_USER_H

#include "types.h"

void system_local_user_init(system_local_user_t *user);
int system_local_user_set_name(system_local_user_t *user, const char *name);
int system_local_user_set_password(system_local_user_t *user, const char *password);
void system_local_user_free(system_local_user_t *user);

int system_local_user_cmp_fn(const void *e1, const void *e2);

#endif // SYSTEM_PLUGIN_AUTHENTICATION_DATA_LOCAL_USER_H