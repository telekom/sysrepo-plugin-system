#include "check.h"
#include "load.h"
#include "common.h"
#include "system/data/ntp/server/list.h"

#include <srpc.h>
#include <utlist.h>
#include <sysrepo.h>

srpc_check_status_t system_ntp_check_server(system_ctx_t *ctx, system_ntp_server_element_t *head)
{
	int error = 0;
	srpc_check_status_t status = srpc_check_status_none;
	size_t contains_count = 0;
	size_t list_elements_count = 0;

	system_ntp_server_element_t *system_server_head = NULL, *server_el = NULL, *found_el = NULL;

	error = system_ntp_load_server(ctx, &system_server_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to load current NTP server list from the system");
		goto error_out;
	}

	// count number of elements
	LL_COUNT(head, server_el, list_elements_count);

	// loaded search list - compare current startup list and the system one
	LL_FOREACH(head, server_el)
	{
		found_el = NULL;

		// find by address
		LL_SEARCH(system_server_head, found_el, server_el, system_ntp_server_element_address_cmp_fn);

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
	// release memory
	system_ntp_server_list_free(&system_server_head);

	return status;
}