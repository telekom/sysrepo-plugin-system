/*
 * telekom / sysrepo-plugin-system
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2021 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <string.h>
#include <sysrepo.h>
#include <arpa/inet.h>
#ifdef SYSTEMD
#include <systemd/sd-bus.h>
#else
#include "utils/dns/resolv_conf.h"
#endif
#include "utils/memory.h"
#include "utils/dns/server.h"
#include "utils/uthash/utlist.h"

static int element_comparator(dns_server_element_t *el1, dns_server_element_t *el2);

void dns_server_init(dns_server_t *s)
{
#ifdef SYSTEMD
	s->addr = (ip_addr_t){
		.af = -1,
	};
#else
	s->addr.value = NULL;
#endif
	s->name = NULL;
	s->port = 0;
	s->delete = false;
}

void dns_server_set_name(dns_server_t *s, char *name)
{
	s->name = xstrdup(name);
}

int dns_server_set_address(dns_server_t *s, char *addr)
{
	int err = 0;
#ifdef SYSTEMD
	if (inet_pton(AF_INET, addr, s->addr.value.ip4) == 1) {
		s->addr.af = AF_INET;
	} else if (inet_pton(AF_INET6, addr, s->addr.value.ip6) == 1) {
		s->addr.af = AF_INET;
	} else {
		// should not be possible -> yang model already checks this, but just in case return an error
		err = -1;
	}
#else
	s->addr.value = xstrdup(addr);
#endif
	return err;
}

static void dns_server_set_delete(dns_server_t *s, bool delete)
{
	s->delete = delete;
}

void dns_server_set_port(dns_server_t *s, int port)
{
	s->port = port;
}

void dns_server_free(dns_server_t *s)
{
	if (s->name) {
		FREE_SAFE(s->name);
	}
#ifndef SYSTEMD
	if (s->addr.value) {
		FREE_SAFE(s->addr.value);
	}
#endif
	dns_server_init(s);
}

int dns_server_list_add(dns_server_element_t **head, char *name)
{
	int err = 0;
	dns_server_element_t *iter = NULL, *new_node = NULL, *search = NULL;

	// search for the name first
	search = (dns_server_element_t *) malloc(sizeof(dns_server_element_t));
	search->server.name = name;
	LL_SEARCH(*head, iter, search, element_comparator);

	if (iter == NULL) {
		// add new server if none found
		new_node = (dns_server_element_t *) malloc(sizeof(dns_server_element_t));
		dns_server_init(&new_node->server);
		dns_server_set_name(&new_node->server, name);
		LL_APPEND(*head, new_node);
	}

	// release search node
	free(search);

	return err;
}

int dns_server_list_delete(dns_server_element_t **head, char *name)
{
	int err = 0;
	dns_server_element_t *iter = NULL, *search = NULL;

	// search for the name first
	search = (dns_server_element_t *) malloc(sizeof(dns_server_element_t));
	search->server.name = name;
	LL_SEARCH(*head, iter, search, element_comparator);

	if (iter) {
		// remove node from the list
		dns_server_set_delete(&iter->server, true);
	} else {
		err = -1;
	}

	free(search);
	return err;
}

int dns_server_list_set_address(dns_server_element_t **head, char *name, char *address)
{
	int err = 0;
	dns_server_element_t *iter = NULL, *search = NULL;

	// search for the name first
	search = (dns_server_element_t *) malloc(sizeof(dns_server_element_t));
	search->server.name = name;
	LL_SEARCH(*head, iter, search, element_comparator);

	if (iter) {
		dns_server_set_address(&iter->server, address);
	} else {
		err = -1;
	}

	free(search);
	return err;
}

int dns_server_list_set_port(dns_server_element_t **head, char *name, int port)
{
	int err = 0;
	dns_server_element_t *iter = NULL, *search = NULL;

	// search for the name first
	search = (dns_server_element_t *) malloc(sizeof(dns_server_element_t));
	search->server.name = name;
	LL_SEARCH(*head, iter, search, element_comparator);

	if (iter) {
		dns_server_set_port(&iter->server, port);
	} else {
		err = -1;
	}

	free(search);
	return err;
}

int dns_server_list_dump(dns_server_element_t **head)
{
	int err = 0;
	dns_server_element_t *iter = NULL;

	// depending on systemd or no systemd -> store information about the DNS server list
#ifdef SYSTEMD
	// store information in the DNS property from Manager on sd-bus
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

	LL_FOREACH(*head, iter)
	{
		dns_server_t *srv = &iter->server;

		if (srv->name == NULL) {
			continue;
		}

		if (srv->delete) {
			dns_server_free(srv);
			continue;
		}

		// enter a struct first
		r = sd_bus_message_open_container(msg, 'r', "iay");
		if (r < 0) {
			goto invalid;
		}

		// set address family
		r = sd_bus_message_append(msg, "i", srv->addr.af);
		if (r < 0) {
			goto invalid;
		}

		// enter array of bytes for an address
		r = sd_bus_message_open_container(msg, 'a', "y");
		if (r < 0) {
			goto invalid;
		}

		// append address bytes accordingly with address family
		switch (srv->addr.af) {
			case AF_INET:
				for (uint8_t j = 0; j < sizeof(srv->addr.value.ip4) / sizeof(uint8_t); j++) {
					r = sd_bus_message_append(msg, "y", srv->addr.value.ip4[j]);
					if (r < 0) {
						goto invalid;
					}
				}
				break;
			case AF_INET6:
				for (uint8_t j = 0; j < sizeof(srv->addr.value.ip6) / sizeof(uint8_t); j++) {
					r = sd_bus_message_append(msg, "y", srv->addr.value.ip6[j]);
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

	SRPLG_LOG_INF(PLUGIN_NAME, "Set DNS servers successfully!");
	goto finish;

invalid:
	SRPLG_LOG_ERR(PLUGIN_NAME, "sd-bus failure: %d, sdb_err contents: '%s'", r, sdb_err.message);
	err = -1;

finish:
	sd_bus_message_unref(msg);
	sd_bus_message_unref(reply);
	sd_bus_flush_close_unref(bus);
#else
	rconf_t cfg;
	rconf_error_t rc_err = 0;
	dns_server_element_t *search = NULL;

	rconf_init(&cfg);

	rc_err = rconf_load_file(&cfg, RESOLV_CONF_PATH);

	if (rc_err != rconf_error_none) {
		goto invalid;
	}

	int count = 0;
	LL_FOREACH(*head, iter)
	{
		dns_server_t *srv = &iter->server;

		if (srv->name == NULL) {
			continue;
		}

		if (srv->delete) {
			rc_err = rconf_remove_nameserver(&cfg, srv->addr.value);
			if (rc_err != rconf_error_none) {
				goto invalid;
			}

			dns_server_free(srv);
			continue;
		}

		rc_err = rconf_set_nameserver(&cfg, count, srv->addr.value, 1);
		if (rc_err != rconf_error_none) {
			goto invalid;
		}
		++count;
	}

	for (int i = 0; i < cfg.nameserver_n; i++) {
		// create search element
		search = malloc(sizeof(*search));
		search->server.name = cfg.nameserver[i];

		LL_SEARCH(*head, iter, search, element_comparator);
		if (iter == NULL) {
			// nameserver not found -> remove it from the config
			rc_err = rconf_remove_nameserver(&cfg, cfg.nameserver[i]);
			if (rc_err != rconf_error_none) {
				goto invalid;
			}

			i--;
		}

		free(search);
	}

	rc_err = rconf_export(&cfg, RESOLV_CONF_PATH);
	if (rc_err != rconf_error_none) {
		goto invalid;
	}

	goto finish;

invalid:
	if (search) {
		free(search);
	}
	err = -1;
finish:
	rconf_free(&cfg);
#endif
	return err;
}

static int element_comparator(dns_server_element_t *el1, dns_server_element_t *el2)
{
	return strcmp(el1->server.name, el2->server.name);
}
