#ifndef SYSTEM_PLUGIN_API_AUTHENTICATION_STORE_H
#define SYSTEM_PLUGIN_API_AUTHENTICATION_STORE_H

#include "context.h"

int system_authentication_store_user(system_ctx_t *ctx, system_local_user_element_t *head);
int system_authentication_store_user_authorized_key(system_ctx_t *ctx, const char *user, system_authorized_key_element_t *head);

#endif // SYSTEM_PLUGIN_API_AUTHENTICATION_STORE_H