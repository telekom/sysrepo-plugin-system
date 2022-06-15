#include "check.h"
#include "load.h"
#include "common.h"
#include "system/data/authentication/local_user/list.h"

#include <sysrepo.h>
#include <utlist.h>

srpc_check_status_t system_authentication_check_user(system_ctx_t *ctx, system_local_user_element_t *head)
{
	int error = 0;
	srpc_check_status_t status = srpc_check_status_none;
	size_t contains_count = 0;
	size_t list_elements_count = 0;

	system_local_user_element_t *system_user_head = NULL, *user_el = NULL, *found_el = NULL;

	error = system_authentication_load_user(ctx, &system_user_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_load_user() error (%d)", error);
		goto error_out;
	}

	// count number of startup elements
	LL_COUNT(head, user_el, list_elements_count);

	// compare
	LL_FOREACH(head, user_el)
	{
		found_el = NULL;

		LL_SEARCH(system_user_head, found_el, user_el, system_local_user_element_cmp_fn);

		if (found_el != NULL) {
			contains_count++;
		}
	}

	if (contains_count == 0) {
		status = srpc_check_status_non_existant;
	} else if (contains_count != list_elements_count) {
		status = srpc_check_status_partial;
	} else {
		status = srpc_check_status_equal;
	}

	goto out;

error_out:
	status = srpc_check_status_error;

out:

	system_local_user_list_free(&system_user_head);

	return status;
}

srpc_check_status_t system_authentication_check_user_authorized_key(system_ctx_t *ctx, const char *user, system_authorized_key_element_t *head)
{
	srpc_check_status_t status = srpc_check_status_none;
	return status;
}