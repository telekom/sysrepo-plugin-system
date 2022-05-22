#include "search.h"
#include "common.h"
#include "utils/memory.h"

#include <systemd/sd-bus.h>
#include <sysrepo.h>

#include <utlist.h>

static int system_search_comparator(void *e1, void *e2);

int system_dns_resolver_load_search_values(system_dns_search_element_t **head)
{
	int error = 0;
	system_dns_search_element_t *new_el = NULL;
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

		// all read normally - allocate new element
		new_el = (system_dns_search_element_t *) malloc(sizeof(system_dns_search_element_t));
		if (!new_el) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "malloc() failed for new_el");
			goto invalid;
		}

		// copy values read from sd-bus
		new_el->search = tmp_search;

		// append to the list
		LL_APPEND(*head, new_el);
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
	system_dns_search_element_t *new_search_el = NULL;

	error = system_dns_resolver_load_search_values(&search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_load_search_values() error (%d)", error);
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
	error = system_dns_resolver_store_search_values(search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_store_search_values() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	system_dns_resolver_free_search_values(&search_head);

	return error;
}

int system_dns_resolver_modify_dns_search(const char *prev_value, const char *new_value)
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
	to_remove_el.search.domain = xstrdup(prev_value);
	if (!to_remove_el.search.domain) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "xstrdup() for domain failed");
		goto error_out;
	}

	LL_SEARCH(search_head, found_el, &to_remove_el, system_search_comparator);

	if (found_el) {
		// search element found -> change its domain
		free((char *) found_el->search.domain);
		found_el->search.domain = xstrdup(new_value);
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
	system_dns_resolver_free_search_values(&search_head);

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
	error = system_dns_resolver_store_search_values(search_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_store_search_values() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	system_dns_resolver_free_search_values(&search_head);

	return error;
}

static int system_search_comparator(void *e1, void *e2)
{
	system_dns_search_element_t *s1 = (system_dns_search_element_t *) e1;
	system_dns_search_element_t *s2 = (system_dns_search_element_t *) e2;

	return strcmp(s1->search.domain, s2->search.domain);
}
