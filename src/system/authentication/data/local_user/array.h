#ifndef SYSTEM_PLUGIN_AUTHENTICATION_DATA_LOCAL_USER_ARRAY_H
#define SYSTEM_PLUGIN_AUTHENTICATION_DATA_LOCAL_USER_ARRAY_H

#include "types.h"

void system_local_user_array_init(UT_array **users);
void system_local_user_array_free(UT_array **users);

#endif // SYSTEM_PLUGIN_AUTHENTICATION_DATA_LOCAL_USER_ARRAY_H