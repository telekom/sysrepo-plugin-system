#include "server.h"
#include "common.h"
#include "utils/memory.h"

// data
#include "data/ip_address.h"
#include "data/dns_resolver/server/list.h"

#include <systemd/sd-bus.h>
#include <sysrepo.h>

#include <utlist.h>

static int system_server_comparator(void *e1, void *e2);

int system_dns_resolver_load_server_values(system_dns_server_element_t **head)
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

		error = system_dns_server_list_add(head, tmp_server);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_server_list_add() error (%d)", error);
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

int system_dns_resolver_store_server_values(system_dns_server_element_t *head)
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

int system_dns_resolver_create_dns_server_address(const char *value)
{
	int error = 0;
	system_dns_server_element_t *servers_head = NULL;

	system_dns_server_t new_server = {0};

	error = system_dns_resolver_load_server_values(&servers_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_gather_search_values() error (%d)", error);
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
	error = system_dns_resolver_store_server_values(servers_head);
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

int system_dns_resolver_modify_dns_server_address(const char *prev_value, const char *new_value)
{
	int error = 0;
	system_dns_server_element_t *servers_head = NULL;
	system_dns_server_element_t *found_el = NULL;
	system_dns_server_element_t to_modify_el = {0};

	error = system_dns_resolver_load_server_values(&servers_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_gather_search_values() error (%d)", error);
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
	error = system_dns_resolver_store_server_values(servers_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_store_server_values() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	system_dns_server_list_free(&servers_head);

	return error;
}

int system_dns_resolver_delete_dns_server_address(const char *value)
{
	int error = 0;
	system_dns_server_element_t *servers_head = NULL;
	system_dns_server_element_t *found_el = NULL;
	system_dns_server_element_t to_remove_el = {0};

	error = system_dns_resolver_load_server_values(&servers_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_gather_search_values() error (%d)", error);
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
	error = system_dns_resolver_store_server_values(servers_head);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_dns_resolver_store_server_values() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	system_dns_server_list_free(&servers_head);

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

	return 2;
}