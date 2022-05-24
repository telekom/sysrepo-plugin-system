#ifndef SYSTEM_PLUGIN_CONTEXT_H
#define SYSTEM_PLUGIN_CONTEXT_H

#include <sysrepo_types.h>

typedef struct system_ctx_s system_ctx_t;

struct system_ctx_s {
	sr_session_ctx_t *startup_session;
};

#endif // SYSTEM_PLUGIN_CONTEXT_H