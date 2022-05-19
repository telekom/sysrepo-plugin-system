#ifndef SYSTEM_PLUGIN_CONTEXT_H
#define SYSTEM_PLUGIN_CONTEXT_H

#include <sysrepo_types.h>

typedef struct system_ctx_s {
	sr_session_ctx_t *startup_session;
} system_ctx_t;

#endif // SYSTEM_PLUGIN_CONTEXT_H