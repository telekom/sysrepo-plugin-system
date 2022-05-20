#include "dns_resolver.h"
#include "common.h"
#include "sysrepo.h"
#include "utils/memory.h"

#include <stdlib.h>

// uthash
#include <utlist.h>

// systemd or no-systemd
#ifdef SYSTEMD
#include <systemd/sd-bus.h>
#else
#endif

// helpers

static int system_gather_search_values(system_dns_search_element_t **head);
static int system_apply_search_values(system_dns_search_element_t *head);
static int system_search_comparator(void *e1, void *e2);

static int system_gather_server_values(system_dns_server_element_t **head);
static int system_apply_server_values(system_dns_server_element_t *head);
static int system_server_comparator(void *e1, void *e2);

static int system_set_dns_server_address(system_dns_server_t *server, const char *ip);

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

int system_add_dns_server(const char *value)
{
	int error = 0;
	system_dns_server_element_t *servers_head = NULL;
	system_dns_server_element_t *new_server_el = NULL;

	error = system_gather_server_values(&servers_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_gather_search_values() error (%d)", error);
		goto error_out;
	}

	// create new element
	new_server_el = (system_dns_server_element_t *) malloc(sizeof(system_dns_server_element_t));
	if (!new_server_el) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "malloc() for new_server_el failed");
		goto error_out;
	}

	// init properties to 0
	new_server_el->server = (system_dns_server_t){0};

	// set IP address + family
	error = system_set_dns_server_address(&new_server_el->server, value);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_set_dns_server_address() failed (%d)", error);
		goto error_out;
	}

	// append element to the list
	LL_APPEND(servers_head, new_server_el);

	// set search values to the system
	error = system_apply_server_values(servers_head);
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

int system_delete_dns_server(const char *value)
{
	int error = 0;
	system_dns_server_element_t *servers_head = NULL;
	system_dns_server_element_t *found_el = NULL;
	system_dns_server_element_t to_remove_el = {0};

	error = system_gather_server_values(&servers_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_gather_search_values() error (%d)", error);
		goto error_out;
	}

	error = system_set_dns_server_address(&to_remove_el.server, value);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_set_dns_server_address() failed (%d)", error);
		goto error_out;
	}

	LL_SEARCH(servers_head, found_el, &to_remove_el, system_server_comparator);

	if (found_el) {
		// delete found element from the list
		LL_DELETE(servers_head, found_el);
	} else {
		// error - unable to find value in the system to remove
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to find search value of %s in the system", value);
		goto error_out;
	}

	// set search values to the system
	error = system_apply_server_values(servers_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_apply_server_values() error (%d)", error);
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

static int system_apply_search_values(system_dns_search_element_t *head)
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

static int system_search_comparator(void *e1, void *e2)
{
	system_dns_search_element_t *s1 = (system_dns_search_element_t *) e1;
	system_dns_search_element_t *s2 = (system_dns_search_element_t *) e2;

	return strcmp(s1->search.domain, s2->search.domain);
}

static int system_gather_server_values(system_dns_server_element_t **head)
{
	int error = 0;

#ifdef SYSTEMD
	int r;
	sd_bus_message *msg = NULL;
	sd_bus_error sdb_err = SD_BUS_ERROR_NULL;
	sd_bus *bus = NULL;
	int tmp_ifindex = 0;
	size_t tmp_length = 0;

	system_dns_server_t tmp_server = {0};
	system_dns_server_element_t *new_el = NULL;

	r = sd_bus_open_system(&bus);
	if (r < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Failed to open system bus: %s\n", strerror(-r));
		goto finish;
	}

	r = sd_bus_get_property(
		bus,
		"org.freedesktop.resolve1",
		"/org/freedesktop/resolve1",
		"org.freedesktop.resolve1.Manager",
		"DNS",
		&sdb_err,
		&msg,
		"a(iiay)");

	if (r < 0) {
		goto invalid;
	}

	r = sd_bus_message_enter_container(msg, 'a', "(iiay)");
	if (r < 0) {
		goto invalid;
	}

	for (;;) {
		const void *data;
		r = sd_bus_message_enter_container(msg, 'r', "iiay");
		if (r < 0) {
			goto invalid;
		}
		if (r == 0) {
			break;
		}
		r = sd_bus_message_read(msg, "ii", &tmp_ifindex, &tmp_server.address.family);
		if (r < 0) {
			goto invalid;
		}

		switch (tmp_server.address.family) {
			case AF_INET:
				r = sd_bus_message_read_array(msg, 'y', (const void **) &data, &tmp_length);
				if (r >= 0) {
					memcpy(tmp_server.address.value.v4, data, tmp_length);
				}
				break;
			case AF_INET6:
				r = sd_bus_message_read_array(msg, 'y', (const void **) &data, &tmp_length);
				if (r >= 0) {
					memcpy(tmp_server.address.value.v6, data, tmp_length);
				}
				break;
			default:
				// unknown address family -> for now abort
				r = -1;
				break;
		}

		if (r < 0) {
			goto invalid;
		}

		r = sd_bus_message_exit_container(msg);
		if (r < 0) {
			goto invalid;
		}

		// create new node
		new_el = (system_dns_server_element_t *) malloc(sizeof(system_dns_server_element_t));
		if (!new_el) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "malloc() failed for new_el");
			goto invalid;
		}

		// set server value
		new_el->server = tmp_server;

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

static int system_apply_server_values(system_dns_server_element_t *head)
{
	int error = 0;
	system_dns_server_element_t *server_iter_el = NULL;

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
		"SetLinkDNS");
	if (r < 0) {
		goto invalid;
	}

	r = sd_bus_message_append(msg, "i", SYSTEMD_IFINDEX);
	if (r < 0) {
		goto invalid;
	}

	// enter array of structs
	r = sd_bus_message_open_container(msg, 'a', "(iay)");
	if (r < 0) {
		goto invalid;
	}

	LL_FOREACH(head, server_iter_el)
	{
		system_dns_server_t *server = &server_iter_el->server;

		// enter a struct first
		r = sd_bus_message_open_container(msg, 'r', "iay");
		if (r < 0) {
			goto invalid;
		}

		// set address family
		r = sd_bus_message_append(msg, "i", server->address.family);
		if (r < 0) {
			goto invalid;
		}

		// enter array of bytes for an address
		r = sd_bus_message_open_container(msg, 'a', "y");
		if (r < 0) {
			goto invalid;
		}

		// append address bytes accordingly with address family
		switch (server->address.family) {
			case AF_INET:
				for (unsigned char j = 0; j < ARRAY_SIZE(server->address.value.v4); j++) {
					r = sd_bus_message_append(msg, "y", server->address.value.v4[j]);
					if (r < 0) {
						goto invalid;
					}
				}
				break;
			case AF_INET6:
				for (unsigned char j = 0; j < ARRAY_SIZE(server->address.value.v6); j++) {
					r = sd_bus_message_append(msg, "y", server->address.value.v6[j]);
					if (r < 0) {
						goto invalid;
					}
				}
				break;
			default:
				break;
		}

		// exit array
		r = sd_bus_message_close_container(msg);
		if (r < 0) {
			goto invalid;
		}

		// exit struct
		r = sd_bus_message_close_container(msg);
		if (r < 0) {
			goto invalid;
		}
	}

	// exit array of structs
	r = sd_bus_message_close_container(msg);
	if (r < 0) {
		goto invalid;
	}

	// finally call created method
	r = sd_bus_call(bus, msg, 0, &sdb_err, &reply);
	if (r < 0) {
		goto invalid;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Set DNS servers successfully.");
	goto finish;

invalid:
	SRPLG_LOG_ERR(PLUGIN_NAME, "sd-bus failure (%d): %s", r, sdb_err.message);
	error = -1;

finish:
	sd_bus_message_unref(msg);
	sd_bus_message_unref(reply);
	sd_bus_flush_close_unref(bus);
#else
#endif

	return error;
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
}

static int system_set_dns_server_address(system_dns_server_t *server, const char *ip)
{
	int error = 0;

#ifdef SYSTEMD
	if (inet_pton(AF_INET, ip, server->address.value.v4) == 1) {
		server->address.family = AF_INET;
	} else if (inet_pton(AF_INET6, ip, server->address.value.v6) == 1) {
		server->address.family = AF_INET;
	} else {
		// should not be possible -> yang model already checks this, but just in case return an error
		error = -1;
	}
#else
#endif

	return error;
}