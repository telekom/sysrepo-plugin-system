#include "change.h"
#include "load.h"
#include "store.h"
#include "types.h"
#include "common.h"

// data
#include "system/data/dns_resolver/server/list.h"
#include "system/data/dns_resolver/search/list.h"
#include "system/data/dns_resolver/search.h"
#include "system/data/dns_resolver/server.h"
#include "system/data/ip_address.h"

#include <sysrepo.h>

#include <utlist.h>

static int system_search_comparator(void *e1, void *e2);
static int system_server_comparator(void *e1, void *e2);

int system_dns_resolver_change_search_create(system_ctx_t *ctx, const char *value)
{
	int error = 0;
	system_dns_search_element_t *search_head = NULL;

	system_dns_search_t new_search = {0};

	error = system_dns_resolver_load_search(ctx, &search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_load_search() error (%d)", error);
		goto error_out;
	}

	// setup new search value
	error = system_dns_search_set_domain(&new_search, value);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_search_set_domain() error (%d)", error);
		goto error_out;
	}

	// add the value to the list
	error = system_dns_search_list_add(&search_head, new_search);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_search_list_add() error (%d)", error);
		goto error_out;
	}

	// set search values to the system
	error = system_dns_resolver_store_search(ctx, search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_store_search() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	system_dns_search_list_free(&search_head);

	return error;
}

int system_dns_resolver_change_search_modify(system_ctx_t *ctx, const char *prev_value, const char *new_value)
{
	int error = 0;
	system_dns_search_element_t *search_head = NULL;
	system_dns_search_element_t *found_el = NULL;
	system_dns_search_element_t to_modify_el = {0};

	error = system_dns_resolver_load_search(ctx, &search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_load_search() error (%d)", error);
		goto error_out;
	}

	// create element for search
	error = system_dns_search_set_domain(&to_modify_el.search, prev_value);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_search_set_domain() error (%d) for %s", error, prev_value);
		goto error_out;
	}

	LL_SEARCH(search_head, found_el, &to_modify_el, system_search_comparator);

	if (found_el) {
		// search element found -> change its domain
		error = system_dns_search_set_domain(&found_el->search, new_value);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_search_set_domain() error (%d) for %s", error, new_value);
			goto error_out;
		}
	} else {
		// error - unable to find value in the system to remove
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to find search value of %s in the system", prev_value);
		goto error_out;
	}

	// set search values to the system
	error = system_dns_resolver_store_search(ctx, search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_store_search() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	system_dns_search_list_free(&search_head);

	return error;
}

int system_dns_resolver_change_search_delete(system_ctx_t *ctx, const char *value)
{
	int error = 0;
	system_dns_search_element_t *search_head = NULL;
	system_dns_search_element_t *found_el = NULL;
	system_dns_search_element_t to_remove_el = {0};

	error = system_dns_resolver_load_search(ctx, &search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_load_search() error (%d)", error);
		goto error_out;
	}

	// create element for search
	error = system_dns_search_set_domain(&to_remove_el.search, value);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_search_set_domain() error (%d) for %s", error, value);
		goto error_out;
	}

	LL_SEARCH(search_head, found_el, &to_remove_el, system_search_comparator);

	if (found_el) {
		// delete found element from the list
		LL_DELETE(search_head, found_el);
	} else {
		// error - unable to find value in the system to remove
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to find search value of %s in the system", value);
		goto error_out;
	}

	// set search values to the system
	error = system_dns_resolver_store_search(ctx, search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_store_search() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	system_dns_search_list_free(&search_head);

	return error;
}

int system_dns_resolver_change_server_create(system_ctx_t *ctx, const char *value)
{
	int error = 0;
	system_dns_server_element_t *servers_head = NULL;

	system_dns_server_t new_server = {0};

	error = system_dns_resolver_load_server(ctx, &servers_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_load_server() error (%d)", error);
		goto error_out;
	}

	// set IP address + family
	error = system_ip_address_from_str(&new_server.address, value);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ip_address_from_str() failed (%d) for address %s", error, value);
		goto error_out;
	}

	// append element to the list
	error = system_dns_server_list_add(&servers_head, new_server);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_server_list_add() failed (%d)", error);
		goto error_out;
	}

	// set search values to the system
	error = system_dns_resolver_store_server(ctx, servers_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_apply_search_values() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	system_dns_server_list_free(&servers_head);

	return error;
}

int system_dns_resolver_change_server_modify(system_ctx_t *ctx, const char *prev_value, const char *new_value)
{
	int error = 0;
	system_dns_server_element_t *servers_head = NULL;
	system_dns_server_element_t *found_el = NULL;
	system_dns_server_element_t to_modify_el = {0};

	error = system_dns_resolver_load_server(ctx, &servers_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_load_server() error (%d)", error);
		goto error_out;
	}

	error = system_ip_address_from_str(&to_modify_el.server.address, prev_value);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ip_address_from_str() failed (%d)", error);
		goto error_out;
	}

	LL_SEARCH(servers_head, found_el, &to_modify_el, system_server_comparator);

	if (found_el) {
		// change the address value of the server
		error = system_ip_address_from_str(&found_el->server.address, new_value);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_ip_address_from_str() failed for %s", new_value);
			goto error_out;
		}
	} else {
		// error - unable to find value in the system to remove
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to find server value of %s in the system", prev_value);
		goto error_out;
	}

	// set search values to the system
	error = system_dns_resolver_store_server(ctx, servers_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_store_server() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	system_dns_server_list_free(&servers_head);

	return error;
}

int system_dns_resolver_change_server_delete(system_ctx_t *ctx, const char *value)
{
	int error = 0;
	system_dns_server_element_t *servers_head = NULL;
	system_dns_server_element_t *found_el = NULL;
	system_dns_server_element_t to_remove_el = {0};

	error = system_dns_resolver_load_server(ctx, &servers_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_load_server() error (%d)", error);
		goto error_out;
	}

	error = system_ip_address_from_str(&to_remove_el.server.address, value);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ip_address_from_str() failed (%d)", error);
		goto error_out;
	}

	LL_SEARCH(servers_head, found_el, &to_remove_el, system_server_comparator);

	if (found_el) {
		// delete found element from the list
		LL_DELETE(servers_head, found_el);
	} else {
		// error - unable to find value in the system to remove
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to find server value of %s in the system", value);
		goto error_out;
	}

	// set search values to the system
	error = system_dns_resolver_store_server(ctx, servers_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_store_server() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	system_dns_server_list_free(&servers_head);

	return error;
}

static int system_search_comparator(void *e1, void *e2)
{
	system_dns_search_element_t *s1 = (system_dns_search_element_t *) e1;
	system_dns_search_element_t *s2 = (system_dns_search_element_t *) e2;

	return strcmp(s1->search.domain, s2->search.domain);
}

static int system_server_comparator(void *e1, void *e2)
{
	system_dns_server_element_t *s1 = (system_dns_server_element_t *) e1;
	system_dns_server_element_t *s2 = (system_dns_server_element_t *) e2;

#ifdef SYSTEMD
	if (s1->server.address.family == s2->server.address.family) {
		switch (s1->server.address.family) {
			case AF_INET:
				return memcmp(s1->server.address.value.v4, s2->server.address.value.v4, sizeof(s1->server.address.value.v4));
				break;
			case AF_INET6:
				return memcmp(s1->server.address.value.v6, s2->server.address.value.v6, sizeof(s1->server.address.value.v6));
				break;
			default:
				return 1;
				break;
		}
	} else {
		return 1;
	}
#else
#endif

	return 2;
}