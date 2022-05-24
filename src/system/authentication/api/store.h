#ifndef SYSTEM_PLUGIN_AUTHENTICATION_API_STORE_H
#define SYSTEM_PLUGIN_AUTHENTICATION_API_STORE_H

#include "context.h"
#include <utarray.h>

int system_authentication_store_user(system_ctx_t *ctx, UT_array *arr);
int system_authentication_store_user_authorized_key(system_ctx_t *ctx, const char *user, UT_array *arr);

#endif // SYSTEM_PLUGIN_AUTHENTICATION_API_STORE_H