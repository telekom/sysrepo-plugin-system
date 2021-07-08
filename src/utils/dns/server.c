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

// helper functions
static dns_server_t *get_server_from_list(dns_server_list_t *list, char *name);
/*static int get_addr_family(char *addr);
static int str_to_ipv4(ip_addr_t *ip, char *addr);
static int str_to_ipv6(ip_addr_t *ip, char *addr);*/

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

void dns_server_list_init(dns_server_list_t *sl)
{
	sl->list = NULL;
	sl->size = 0;
}

int dns_server_list_add_server(dns_server_list_t *sl, char *name)
{
	int err = 0;
	bool name_found = false;

	for (int i = 0; i < sl->size; i++) {
		if (sl->list[i].name == NULL) {
			continue;
		}

		if (strcmp(sl->list[i].name, name) == 0) {
			name_found = true;
			break;
		}
	}

	if (!name_found) {
		int pos = sl->size;
		for (int i = 0; i < sl->size; i++) {
			if (sl->list[i].name == NULL) {
				pos = i;
				break;
			}
		}

		if (pos == sl->size) {
			sl->list = xrealloc(sl->list, sizeof(dns_server_t) * (unsigned long) (sl->size + 1));
		}
		dns_server_init(&sl->list[pos]);
		dns_server_set_name(&sl->list[pos], name);
		if (pos == sl->size) {
			++sl->size;
		}
	}

	return err;
}

int dns_server_list_set_server_delete(dns_server_list_t *sl, char *name)
{
	int err = 0;
	dns_server_t *srv = NULL;

	srv = get_server_from_list(sl, name);
	if (srv == NULL) {
		// unable to set value to unexisting server -> quit
		err = -1;
		goto out;
	}

	dns_server_set_delete(srv, true);
out:
	return err;
}

int dns_server_list_set_address(dns_server_list_t *sl, char *name, char *address)
{
	int err = 0;
	dns_server_t *srv = NULL;

	srv = get_server_from_list(sl, name);
	if (srv == NULL) {
		// unable to set value to unexisting server -> quit
		err = -1;
		goto out;
	}

	err = dns_server_set_address(srv, address);
out:
	return err;
}

int dns_server_list_set_port(dns_server_list_t *sl, char *name, int port)
{
	int err = 0;
	dns_server_t *srv = NULL;

	srv = get_server_from_list(sl, name);
	if (srv == NULL) {
		// unable to set value to unexisting server -> quit
		err = -1;
		goto out;
	}

	dns_server_set_port(srv, port);
out:
	return err;
}

int dns_server_list_dump_config(dns_server_list_t *sl)
{
	int err = 0;
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
		SRP_LOG_ERR("Failed to open system bus: %s\n", strerror(-r));
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

	for (int i = 0; i < sl->size; i++) {
		dns_server_t *srv = sl->list + i;

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

	SRP_LOG_INFMSG("Set DNS servers successfully!");
	goto finish;

invalid:
	SRP_LOG_ERR("sd-bus failure: %d, sdb_err contents: '%s'", r, sdb_err.message);
	err = -1;

finish:
	sd_bus_message_unref(msg);
	sd_bus_message_unref(reply);
	sd_bus_flush_close_unref(bus);
#else
	rconf_t cfg;
	rconf_error_t rc_err = 0;

	rconf_init(&cfg);

	rc_err = rconf_load_file(&cfg, RESOLV_CONF_PATH);

	if (rc_err != rconf_error_none) {
		goto invalid;
	}

	for (int i = 0; i < sl->size; i++) {
		if (sl->list[i].name == NULL) {
			continue;
		}

		if (sl->list[i].delete) {
			rc_err = rconf_remove_nameserver(&cfg, sl->list[i].addr.value);
			if (rc_err != rconf_error_none) {
				goto invalid;
			}

			dns_server_free(&sl->list[i]);

			continue;
		}

		rc_err = rconf_set_nameserver(&cfg, i, sl->list[i].addr.value, 1);
		if (rc_err != rconf_error_none) {
			goto invalid;
		}
	}

	for (int i = 0; i < cfg.nameserver_n; i++) {
		bool found = false;
		for (int j = 0; j < sl->size; j++) {
			if (sl->list[j].addr.value == NULL) {
				continue;
			}

			if (strcmp(sl->list[j].addr.value, cfg.nameserver[i]) == 0) {
				found = true;
				break;
			}

		}

		if (!found) {
			rc_err = rconf_remove_nameserver(&cfg, cfg.nameserver[i]);
			if (rc_err != rconf_error_none) {
				goto invalid;
			}

			i--;
		}
	}

	rc_err = rconf_export(&cfg, RESOLV_CONF_PATH);
	if (rc_err != rconf_error_none) {
		goto invalid;
	}

	goto finish;

invalid:
	err = -1;
finish:
	rconf_free(&cfg);
#endif
	return err;
}

void dns_server_list_free(dns_server_list_t *sl)
{
	if (sl->list != NULL) {
		for (int i = 0; i < sl->size; i++) {
			dns_server_free(&sl->list[i]);
		}
		FREE_SAFE(sl->list);
		dns_server_list_init(sl);
	}
}

static dns_server_t *get_server_from_list(dns_server_list_t *list, char *name)
{
	dns_server_t *srv = NULL;

	for (int i = 0; i < list->size; i++) {
		if (list->list[i].name == NULL) {
			continue;
		}

		if (strcmp(list->list[i].name, name) == 0) {
			srv = list->list + i;
			break;
		}
	}

	return srv;
}

/*
static int get_addr_family(char *addr)
{
	int af = -1;
	char buf[16] = {0};

	if (inet_pton(AF_INET, addr, buf) == 1) {
		af = AF_INET;
	} else if (inet_pton(AF_INET6, addr, buf) == 1) {
		af = AF_INET6;
	}
	return af;
}

static int str_to_ipv4(ip_addr_t *ip, char *addr)
{
	int err = 0;
	// format: x.x.x.x -> split by '.' and convert bytes
	const char *delim = ".";
	char *token = NULL;
	int i = 0;

	token = strtok(addr, delim);
	while (token != NULL) {
		const int val = atoi(token);
		if (val >= 0 && val < 256) {
			if (i >= 4) {
				// x.x.x.x.x -> wrong format
				err = -1;
				goto out;
			}
			ip->value.ip4[i] = (uint8_t) val;
			i++;
		} else {
			err = -1;
			goto out;
		}
		token = strtok(NULL, delim);
	}
out:
	return err;
}

static int str_to_ipv6(ip_addr_t *ip, char *addr)
{
	int err = 0;
	// format: abcd:abcd:...:abcd (16B)
	// ab => 1 byte, cd => 1 byte
	const char *delim = ":";
	char *token = NULL;
	const uint8_t byte_val = 0xff;
	int val = 0;
	uint8_t first = 0;
	uint8_t second = 0;
	int i = 0;

	token = strtok(addr, delim);
	while (token != NULL) {
		val = atoi(token);
		first = (uint8_t) (val & (byte_val << 8));
		second = (uint8_t) (val & (byte_val));
		if (i >= 16) {
			err = -1;
			goto out;
		}

		ip->value.ip6[i] = first;
		ip->value.ip6[i + 1] = second;
		i += 2;
		token = strtok(NULL, delim);
	}
out:
	return err;
}
*/
