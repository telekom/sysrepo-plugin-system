#include "dns_resolver.h"
#include "common.h"
#include "sysrepo.h"
#include "utils/memory.h"

#include <stdlib.h>

// uthash
#include <utlist.h>

// helpers

static int system_gather_search_values(system_dns_search_element_t **head);
static int system_apply_search_values(system_dns_search_element_t *head);
static int system_search_comparator(void *e1, void *e2);

////

int system_add_dns_search(const char *value)
{
	int error = 0;
	system_dns_search_element_t *search_head = NULL;
	system_dns_search_element_t *new_search_el = NULL;

	error = system_gather_search_values(&search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_gather_search_values() error (%d)", error);
		goto error_out;
	}

	// create new element
	new_search_el = (system_dns_search_element_t *) malloc(sizeof(system_dns_search_element_t));
	if (!new_search_el) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "malloc() for new_search_el failed");
		goto error_out;
	}

	new_search_el->search.domain = xstrdup(value);
	if (!new_search_el->search.domain) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "xstrdup() for domain failed");
		goto error_out;
	}

	// append element to the list
	LL_APPEND(search_head, new_search_el);

	// set search values to the system
	error = system_apply_search_values(search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_apply_search_values() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	return error;
}

int system_delete_dns_search(const char *value)
{
	int error = 0;
	system_dns_search_element_t *search_head = NULL;
	system_dns_search_element_t *found_el = NULL;
	system_dns_search_element_t to_remove_el = {0};

	error = system_gather_search_values(&search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_gather_search_values() error (%d)", error);
		goto error_out;
	}

	// create element for search
	to_remove_el.search.domain = xstrdup(value);
	if (!to_remove_el.search.domain) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "xstrdup() for domain failed");
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
	error = system_apply_search_values(search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_apply_search_values() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	return error;
}

static int system_gather_search_values(system_dns_search_element_t **head)
{
	int error = 0;
	return error;
}

static int system_apply_search_values(system_dns_search_element_t *head)
{
	int error = 0;
	return error;
}

static int system_search_comparator(void *e1, void *e2)
{
	system_dns_search_element_t *s1 = (system_dns_search_element_t *) e1;
	system_dns_search_element_t *s2 = (system_dns_search_element_t *) e2;

	return strcmp(s1->search.domain, s2->search.domain);
}