/*
 * telekom / sysrepo-plugin-system
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2022 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "check.h"
#include "load.h"
#include "srpc/types.h"
#include "types.h"
#include "common.h"
#include "system/data/dns_resolver/search/list.h"
#include "system/data/dns_resolver/server/list.h"

#include <sysrepo.h>

#include <utlist.h>

srpc_check_status_t system_dns_resolver_check_search(system_ctx_t *ctx, system_dns_search_element_t *head)
{
	int error = 0;
	srpc_check_status_t status = srpc_check_status_none;
	size_t contains_count = 0;
	size_t list_elements_count = 0;

	system_dns_search_element_t *system_search_head = NULL, *search_el = NULL, *found_el = NULL;

	error = system_dns_resolver_load_search(ctx, &system_search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to load current dns-resolver search leaf-list from the system");
		goto error_out;
	}

	// count number of elements
	LL_COUNT(head, search_el, list_elements_count);

	// loaded search list - compare current startup list and the system one
	LL_FOREACH(head, search_el)
	{
		found_el = NULL;

		LL_SEARCH(system_search_head, found_el, search_el, system_dns_search_element_cmp_fn);

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
	system_dns_search_list_free(&system_search_head);

	return status;
}

srpc_check_status_t system_dns_resolver_check_server(system_ctx_t *ctx, system_dns_server_element_t *head)
{
	int error = 0;
	srpc_check_status_t status = srpc_check_status_none;
	size_t contains_count = 0;
	size_t list_elements_count = 0;

	system_dns_server_element_t *system_server_head = NULL, *server_el = NULL, *found_el = NULL;

	error = system_dns_resolver_load_server(ctx, &system_server_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to load current dns-resolver server list from the system");
		goto error_out;
	}

	// count number of elements
	LL_COUNT(head, server_el, list_elements_count);

	// loaded search list - compare current startup list and the system one
	LL_FOREACH(head, server_el)
	{
		found_el = NULL;

		LL_SEARCH(system_server_head, found_el, server_el, system_dns_server_element_cmp_fn);

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
	system_dns_server_list_free(&system_server_head);

	return status;
}
