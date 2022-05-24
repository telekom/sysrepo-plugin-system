#ifndef SYSTEM_PLUGIN_AUTHENTICATION_API_LOAD_H
#define SYSTEM_PLUGIN_AUTHENTICATION_API_LOAD_H

#include "context.h"
#include <utarray.h>

int system_authentication_load_user(system_ctx_t *ctx, UT_array **arr);

#endif // SYSTEM_PLUGIN_AUTHENTICATION_API_LOAD_H