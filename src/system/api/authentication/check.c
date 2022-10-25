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
#include "plugin/common.h"
#include "system/data/authentication/local_user/list.h"
#include "system/data/authentication/authorized_key/list.h"

#include <sysrepo.h>
#include <utlist.h>

srpc_check_status_t system_authentication_check_user(system_ctx_t *ctx, system_local_user_element_t *head, system_local_user_element_t **system_head)
{
	int error = 0;
	srpc_check_status_t status = srpc_check_status_none;
	size_t contains_count = 0;
	size_t list_elements_count = 0;

	system_local_user_element_t *user_el = NULL, *found_el = NULL;

	error = system_authentication_load_user(ctx, system_head);
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

		LL_SEARCH(*system_head, found_el, user_el, system_local_user_element_cmp_fn);

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

	return status;
}

srpc_check_status_t system_authentication_check_user_authorized_key(system_ctx_t *ctx, const char *user, system_authorized_key_element_t *head)
{
	int error = 0;
	srpc_check_status_t status = srpc_check_status_none;
	size_t contains_count = 0;
	size_t list_elements_count = 0;

	system_authorized_key_element_t *system_key_head = NULL, *key_el = NULL, *found_el = NULL;

	error = system_authentication_load_user_authorized_key(ctx, user, &system_key_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_load_user_authorized_key() error (%d)", error);
		goto error_out;
	}

	// count number of startup elements
	LL_COUNT(head, key_el, list_elements_count);

	// compare
	LL_FOREACH(head, key_el)
	{
		found_el = NULL;

		LL_SEARCH(system_key_head, found_el, key_el, system_authorized_key_element_cmp_fn);

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

	system_authorized_key_list_free(&system_key_head);

	return status;
}