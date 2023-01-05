#include "store.h"

static int system_startup_store_hostname(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);
static int system_startup_store_ntp(void *priv, sr_session_ctx_t *session, const struct ly_ctx *ly_ctx, struct lyd_node *parent_node);

int system_aug_running_ds_store(system_ctx_t *ctx, sr_session_ctx_t *session)
{
	int error = 0;
	return error;
}