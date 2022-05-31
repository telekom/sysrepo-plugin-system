#ifndef SYSTEM_PLUGIN_API_AUTHENTICATION_LOAD_H
#define SYSTEM_PLUGIN_API_AUTHENTICATION_LOAD_H

#include "context.h"
#include "types.h"
#include <utarray.h>

int system_authentication_load_user(system_ctx_t *ctx, UT_array **arr);
int system_authentication_load_user_authorized_key(system_ctx_t *ctx, const char *user, UT_array **arr);

#endif // SYSTEM_PLUGIN_API_AUTHENTICATION_LOAD_H