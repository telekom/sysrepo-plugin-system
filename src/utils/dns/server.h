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

#ifndef DNS_SERVER_H
#define DNS_SERVER_H

#include <bits/types/res_state.h>
#include <stdbool.h>

typedef struct ip_addr_s ip_addr_t;
typedef struct dns_server_s dns_server_t;
typedef struct dns_server_element_s dns_server_element_t;
typedef struct dns_server_list_s dns_server_list_t;
typedef union ip_addr_value_u ip_addr_value_t;

union ip_addr_value_u {
	uint8_t ip4[4];
	uint8_t ip6[16];
};

struct ip_addr_s {
#ifdef SYSTEMD
	int af;
	ip_addr_value_t value;
#else
	char *value;
#endif
};

struct dns_server_s {
	char *name;
	ip_addr_t addr;
	int port;
	bool delete;
};

struct dns_server_element_s {
	dns_server_t server;
	struct dns_server_element_s *next;
};

// server functions
void dns_server_init(dns_server_t *s);
void dns_server_set_name(dns_server_t *s, char *name);
int dns_server_set_address(dns_server_t *s, char *addr);
void dns_server_set_port(dns_server_t *s, int port);
void dns_server_free(dns_server_t *s);

// server list
int dns_server_list_add(dns_server_element_t **head, char *name);
int dns_server_list_delete(dns_server_element_t **head, char *name);
int dns_server_list_set_address(dns_server_element_t **head, char *name, char *address);
int dns_server_list_set_port(dns_server_element_t **head, char *name, int port);
int dns_server_list_dump(dns_server_element_t **head);

#endif // DNS_SERVER_H
