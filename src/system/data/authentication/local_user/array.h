#ifndef SYSTEM_PLUGIN_DATA_AUTHENTICATION_LOCAL_USER_ARRAY_H
#define SYSTEM_PLUGIN_DATA_AUTHENTICATION_LOCAL_USER_ARRAY_H

#include "types.h"

void system_local_user_array_init(UT_array **users);
int system_local_user_array_add(UT_array **users, system_local_user_t user);
void system_local_user_array_free(UT_array **users);

#endif // SYSTEM_PLUGIN_DATA_AUTHENTICATION_LOCAL_USER_ARRAY_H