#ifndef SYSTEM_PLUGIN_API_AUTHENTICATION_CHECK_H
#define SYSTEM_PLUGIN_API_AUTHENTICATION_CHECK_H

#include "context.h"
#include "types.h"
#include <srpc.h>

srpc_check_status_t system_authentication_check_user(system_ctx_t *ctx, system_local_user_element_t *head, system_local_user_element_t **system_head);
srpc_check_status_t system_authentication_check_user_authorized_key(system_ctx_t *ctx, const char *user, system_authorized_key_element_t *head);

#endif // SYSTEM_PLUGIN_API_AUTHENTICATION_CHECK_H