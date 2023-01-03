#include "plugin.h"

int sr_plugin_init_cb(sr_session_ctx_t *session, void **private_data)
{
	int error = SR_ERR_OK;
	return error;
}

void sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *private_data)
{
}
