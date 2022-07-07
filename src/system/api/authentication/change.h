#ifndef SYSTEM_PLUGIN_API_AUTHENTICATION_CHANGE_H
#define SYSTEM_PLUGIN_API_AUTHENTICATION_CHANGE_H

#include "context.h"

#include <srpc.h>

int system_authentication_change_user_name(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx);
int system_authentication_change_user_password(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx);
int system_authentication_change_user_authorized_key(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx);

#endif // SYSTEM_PLUGIN_API_AUTHENTICATION_CHANGE_H