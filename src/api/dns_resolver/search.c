#include "search.h"
#include "common.h"
#include "data/dns_resolver/search.h"
#include "data/dns_resolver/search/list.h"
#include "utils/memory.h"

#include <systemd/sd-bus.h>
#include <sysrepo.h>

#include <utlist.h>

static int system_search_comparator(void *e1, void *e2);

int system_dns_resolver_load_search_values(system_dns_search_element_t **head)
{
	int error = 0;
	system_dns_search_t tmp_search = {0};

#ifdef SYSTEMD
	int r;
	sd_bus_message *msg = NULL;
	sd_bus_error sdb_err = SD_BUS_ERROR_NULL;
	sd_bus *bus = NULL;

	r = sd_bus_open_system(&bus);
	if (r < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Failed to open system bus: %s\n", strerror(-r));
		goto invalid;
	}

	r = sd_bus_get_property(
		bus,
		"org.freedesktop.resolve1",
		"/org/freedesktop/resolve1",
		"org.freedesktop.resolve1.Manager",
		"Domains",
		&sdb_err,
		&msg,
		"a(isb)");

	if (r < 0) {
		goto invalid;
	}

	// message recieved -> enter msg and get needed info
	r = sd_bus_message_enter_container(msg, 'a', "(isb)");
	if (r < 0) {
		goto invalid;
	}

	for (;;) {
		r = sd_bus_message_enter_container(msg, 'r', "isb");
		if (r < 0) {
			goto invalid;
		}

		if (r == 0) {
			// done with reading data
			break;
		}

		// read Domain struct
		r = sd_bus_message_read(msg, "isb", &tmp_search.ifindex, &tmp_search.domain, &tmp_search.search);
		if (r < 0) {
			goto invalid;
		}

		// leave Domain struct
		r = sd_bus_message_exit_container(msg);
		if (r < 0) {
			goto invalid;
		}

		error = system_dns_search_list_add(head, tmp_search);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_search_list_add() error (%d)", error);
			goto invalid;
		}
	}

	goto finish;

invalid:
	SRPLG_LOG_ERR(PLUGIN_NAME, "sd-bus failure (%d): %s", r, sdb_err.message);
	error = -1;

finish:
	sd_bus_message_unref(msg);
	sd_bus_flush_close_unref(bus);
#else
#endif

	return error;
}

int system_dns_resolver_store_search_values(system_dns_search_element_t *head)
{
	int error = 0;
	system_dns_search_element_t *search_iter_el = NULL;

#ifdef SYSTEMD
	int r;
	sd_bus_error sdb_err = SD_BUS_ERROR_NULL;
	sd_bus_message *msg = NULL;
	sd_bus_message *reply = NULL;
	sd_bus *bus = NULL;

	r = sd_bus_open_system(&bus);
	if (r < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Failed to open system bus: %s\n", strerror(-r));
		goto invalid;
	}

	r = sd_bus_message_new_method_call(
		bus,
		&msg,
		"org.freedesktop.resolve1",
		"/org/freedesktop/resolve1",
		"org.freedesktop.resolve1.Manager",
		"SetLinkDomains");
	if (r < 0) {
		goto invalid;
	}

	// set ifindex to the first value in the list
	r = sd_bus_message_append(msg, "i", SYSTEMD_IFINDEX);
	if (r < 0) {
		goto invalid;
	}

	r = sd_bus_message_open_container(msg, 'a', "(sb)");
	if (r < 0) {
		goto invalid;
	}

	LL_FOREACH(head, search_iter_el)
	{
		r = sd_bus_message_append(msg, "(sb)", search_iter_el->search.domain, search_iter_el->search.search);
		if (r < 0) {
			goto invalid;
		}
	}

	r = sd_bus_message_close_container(msg);
	if (r < 0) {
		goto invalid;
	}

	r = sd_bus_call(bus, msg, 0, &sdb_err, &reply);
	if (r < 0) {
		goto invalid;
	}

	//SRP_LOG_INF("Set domains successfully!");
	goto finish;

invalid:
	SRPLG_LOG_ERR(PLUGIN_NAME, "sd-bus failure (%d): %s", r, sdb_err.message);
	error = -1;

finish:
	sd_bus_message_unref(msg);
	sd_bus_flush_close_unref(bus);
#else
#endif

	return error;
}

void system_dns_resolver_free_search_values(system_dns_search_element_t **head)
{
	system_dns_search_element_t *iter_el = NULL, *tmp_el = NULL;
	LL_FOREACH_SAFE(*head, iter_el, tmp_el)
	{
		LL_DELETE(*head, iter_el);
		if (iter_el->search.domain) {
			free((void *) iter_el->search.domain);
		}
		free(iter_el);
	}
}

int system_dns_resolver_create_dns_search(const char *value)
{
	int error = 0;
	system_dns_search_element_t *search_head = NULL;

	system_dns_search_t new_search = {0};

	error = system_dns_resolver_load_search_values(&search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_load_search_values() error (%d)", error);
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
	error = system_dns_resolver_store_search_values(search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_store_search_values() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	system_dns_search_list_free(&search_head);

	return error;
}

int system_dns_resolver_modify_dns_search(const char *prev_value, const char *new_value)
{
	int error = 0;
	system_dns_search_element_t *search_head = NULL;
	system_dns_search_element_t *found_el = NULL;
	system_dns_search_element_t to_modify_el = {0};

	error = system_dns_resolver_load_search_values(&search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_load_search_values() error (%d)", error);
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
	error = system_dns_resolver_store_search_values(search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_store_search_values() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	system_dns_search_list_free(&search_head);

	return error;
}

int system_dns_resolver_delete_dns_search(const char *value)
{
	int error = 0;
	system_dns_search_element_t *search_head = NULL;
	system_dns_search_element_t *found_el = NULL;
	system_dns_search_element_t to_remove_el = {0};

	error = system_dns_resolver_load_search_values(&search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_load_search_values() error (%d)", error);
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
	error = system_dns_resolver_store_search_values(search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_store_search_values() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	system_dns_search_list_free(&search_head);

	return error;
}

static int system_search_comparator(void *e1, void *e2)
{
	system_dns_search_element_t *s1 = (system_dns_search_element_t *) e1;
	system_dns_search_element_t *s2 = (system_dns_search_element_t *) e2;

	return strcmp(s1->search.domain, s2->search.domain);
}
