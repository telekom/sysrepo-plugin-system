#ifndef SYSTEM_PLUGIN_CONTEXT_H
#define SYSTEM_PLUGIN_CONTEXT_H

#include "types.h"
#include <sysrepo_types.h>

typedef struct system_ctx_s system_ctx_t;

struct system_ctx_s {
	sr_session_ctx_t *startup_session;
	system_dns_search_element_t *temp_search_head; ///< Allocated before changes iteration and free'd after.
	system_dns_server_element_t *temp_server_head; ///< Allocated before changes iteration and free'd after.
};

#endif // SYSTEM_PLUGIN_CONTEXT_H