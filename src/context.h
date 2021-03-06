#ifndef SYSTEM_PLUGIN_CONTEXT_H
#define SYSTEM_PLUGIN_CONTEXT_H

#include "types.h"
#include "umgmt/types.h"
#include <sysrepo_types.h>

#include <umgmt.h>

typedef struct system_ctx_s system_ctx_t;

struct system_ctx_s {
	sr_session_ctx_t *startup_session;
	system_dns_search_element_t *temp_dns_search;  ///< Allocated before changes iteration and free'd after.
	system_dns_server_element_t *temp_dns_servers; ///< Allocated before changes iteration and free'd after.
	system_ntp_server_element_t *temp_ntp_servers; ///< Allocated before changes iteration and free'd after.
	struct {
		system_local_user_element_t *created;
		system_local_user_element_t *modified;
		system_local_user_element_t *deleted;
		struct {
			system_local_user_element_t *created;
			system_local_user_element_t *modified;
			system_local_user_element_t *deleted;
		} keys;
	} temp_users; ///< Users created/modified/deleted during change callbacks. After changes the user modifications are applied on the system values.
};

#endif // SYSTEM_PLUGIN_CONTEXT_H