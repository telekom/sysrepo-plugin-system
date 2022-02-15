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

#ifndef NTP_SERVER_LIST_H_ONCE
#define NTP_SERVER_LIST_H_ONCE
#include <inttypes.h>
#include <stdbool.h>
#include <sysrepo.h>
#include "sysrepo_types.h"
#include "utils/uthash/utarray.h"

#define NTP_MAX_SERVERS 20 // TODO: update if needed
#define NTP_CONFIG_FILE "/etc/ntp.conf"
#define NTP_TEMP_FILE "/tmp/tmp_ntp.conf"
#define NTP_BAK_FILE "/etc/ntp.conf.bak"
#define NTP_MAX_ADDR_LEN 45
#define NTP_MAX_PORT_LEN 5
#define NTP_MAX_IBURST_LEN 6
#define NTP_MAX_PREFER_LEN 6
#define NTP_MAX_ENTRY_LEN 100

typedef struct ntp_server_list_s ntp_server_list_t;
typedef struct ntp_server_s ntp_server_t;

struct ntp_server_s {
	char *name;
	char *address;
	char *port;
	char *assoc_type;
	char *iburst;
	char *prefer;
	bool delete;
};

struct ntp_server_list_s {
	ntp_server_t servers[NTP_MAX_SERVERS];
	uint8_t count;
};

extern int ntp_get_server_name(char **name, char *address);
extern int ntp_set_entry_datastore(sr_session_ctx_t *session, ntp_server_t *server_entry);
int ntp_parse_config(ntp_server_t *server_entry, char *line);

int ntp_server_array_init(sr_session_ctx_t *session, UT_array **servers);
int ntp_server_array_add_existing_servers(sr_session_ctx_t *session, UT_array **servers);
int ntp_add_server_entry_to_array(UT_array **servers, ntp_server_t *server_entry);
int ntp_server_array_add_server(UT_array **servers, char *name);
int ntp_server_array_set_address(UT_array **servers, char *name, char *address);
int ntp_server_array_set_port(UT_array **servers, char *name, char *port);
int ntp_server_array_set_assoc_type(UT_array **servers, char *name, char *assoc_type);
int ntp_server_array_set_iburst(UT_array **servers, char *name, char *iburst);
int ntp_server_array_set_prefer(UT_array **servers, char *name, char *prefer);
int ntp_server_array_set_delete(UT_array **servers, char *name, bool delete_val);
void ntp_server_array_free(UT_array **servers);
int save_ntp_config(UT_array **servers);

#endif /* NTP_SERVER_LIST_H_ONCE */
