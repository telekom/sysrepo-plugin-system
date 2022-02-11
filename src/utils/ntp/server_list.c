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

#include "server_list.h"
#include "../memory.h"
#include "utils/uthash/utarray.h"
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

void ntp_server_copy_fn(void *dst, const void *src);
void ntp_server_dtor_fn(void *elt);
int ntp_server_cmp_fn(const void *p1, const void *p2);

void ntp_server_init(ntp_server_t *s)
{
	s->name = NULL;
	s->address = NULL;
	s->port = NULL;
	s->assoc_type = NULL;
	s->iburst = NULL;
	s->prefer = NULL;
	s->delete = false;
}

void ntp_server_set_name(ntp_server_t *s, char *name)
{
	s->name = name ? xstrdup(name) : NULL;
}

void ntp_server_free(ntp_server_t *s)
{
	if (s->name) {
		FREE_SAFE(s->name);
	}

	if (s->address) {
		FREE_SAFE(s->address);
	}

	if (s->port) {
		FREE_SAFE(s->port);
	}

	if (s->assoc_type) {
		FREE_SAFE(s->assoc_type);
	}

	if (s->iburst) {
		FREE_SAFE(s->iburst);
	}

	if (s->prefer) {
		FREE_SAFE(s->prefer);
	}

	s->delete = false;
}

int ntp_server_array_init(sr_session_ctx_t *session, UT_array *servers)
{
	int error = 0;

	UT_icd ntp_servers_icd = {sizeof(ntp_server_t), .init = NULL, .copy = ntp_server_copy_fn, .dtor = ntp_server_dtor_fn};
	utarray_new(servers, &ntp_servers_icd);
	utarray_reserve(servers, NTP_MAX_SERVERS);

	error = ntp_server_array_add_existing_servers(session, servers);

	return error;
}

int ntp_server_array_add_existing_servers(sr_session_ctx_t *session, UT_array *servers)
{
	int error = 0;
	FILE *fp = NULL;
	char *line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	ntp_server_t server_entry = {0};

	// open ntp.conf file for reading
	fp = fopen(NTP_CONFIG_FILE, "r");
	if (fp == NULL) {
		goto error_out;
	}

	// iterate through NTP_CONFIG_FILE
	while ((read = getline(&line, &len, fp)) != -1) {
		// if a line starts with server/pool/peer parse it
		if (strcmp(&line[0], "#") != 0 &&
			(strncmp(line, "server", strlen("server")) == 0 ||
			 strncmp(line, "peer", strlen("peer")) == 0 ||
			 strncmp(line, "pool", strlen("pool")) == 0)) {

			// remove the newline char from line
			line[strlen(line) - 1] = '\0';

			// parse the line for assoc_type, server_name, address, port, iburst, prefer
			error = ntp_parse_config(&server_entry, line);
			if (error != 0) {
				goto error_out;
			}

			// save server entry to internal ntp server list
			error = ntp_add_server_entry_to_array(servers, &server_entry);
			if (error != 0) {
				goto error_out;
			}

			// check if name is same as address
			if (strncmp(server_entry.name, server_entry.address, strlen(server_entry.address)) == 0) {
				// this means that the server entry was already in the /etc/ntp.conf
				// and since we don't know the name, we set it to match the address
				// now we have to save the entry to the datastore as well
				error = ntp_set_entry_datastore(session, &server_entry);
				if (error != 0) {
					goto error_out;
				}
			}

			// free allocated server_entry members
			ntp_server_free(&server_entry);
		}
	}

	FREE_SAFE(line);
	fclose(fp);

	return 0;

error_out:
	if (line != NULL) {
		FREE_SAFE(line);
	}

	if (fp != NULL) {
		fclose(fp);
	}

	ntp_server_free(&server_entry);

	return -1;
}

int ntp_add_server_entry_to_array(UT_array *servers, ntp_server_t *server_entry)
{
	int error = 0;
	unsigned len = 0;
	ntp_server_t *found = NULL;

	len = utarray_len(servers);
	if (len + 1 >= NTP_MAX_SERVERS) {
		return EINVAL;
	}

	found = utarray_find(servers, server_entry, ntp_server_cmp_fn);
	if (found == NULL) {
		utarray_push_back(servers, server_entry);
	}

	return error;
}

int ntp_server_array_add_server(UT_array *servers, char *name)
{
	int error = 0;

	ntp_server_t server = {0};

	// set name for copy() function
	server.name = name;

	// add new server -> sort when added because of utarray_find() function later
	utarray_push_back(servers, &server);
	utarray_sort(servers, ntp_server_cmp_fn);

	return error;
}

int ntp_server_array_set_address(UT_array *servers, char *name, char *address)
{
	ntp_server_t find = {0}, *found = NULL;
	find.name = name;

	found = utarray_find(servers, &find, ntp_server_cmp_fn);
	if (found) {
		if (found->address) {
			FREE_SAFE(found->address);
		}
		found->address = xstrdup(address);
	} else {
		return -1;
	}

	return 0;
}

int ntp_server_array_set_port(UT_array *servers, char *name, char *port)
{
	ntp_server_t find = {0}, *found = NULL;
	find.name = name;

	found = utarray_find(servers, &find, ntp_server_cmp_fn);
	if (found) {
		if (found->port) {
			FREE_SAFE(found->port);
		}
		found->port = xstrdup(port);
	} else {
		return -1;
	}

	return 0;
}

int ntp_server_array_set_assoc_type(UT_array *servers, char *name, char *assoc_type)
{
	ntp_server_t find = {0}, *found = NULL;
	find.name = name;

	found = utarray_find(servers, &find, ntp_server_cmp_fn);
	if (found) {
		if (found->assoc_type) {
			FREE_SAFE(found->assoc_type);
		}
		found->assoc_type = xstrdup(assoc_type);
	} else {
		return -1;
	}

	return 0;
}

int ntp_server_array_set_iburst(UT_array *servers, char *name, char *iburst)
{
	ntp_server_t find = {0}, *found = NULL;
	find.name = name;

	found = utarray_find(servers, &find, ntp_server_cmp_fn);
	if (found) {
		if (found->iburst) {
			FREE_SAFE(found->iburst);
		}
		found->iburst = xstrdup(iburst);
	} else {
		return -1;
	}

	return 0;
}

int ntp_server_array_set_prefer(UT_array *servers, char *name, char *prefer)
{
	ntp_server_t find = {0}, *found = NULL;
	find.name = name;

	found = utarray_find(servers, &find, ntp_server_cmp_fn);
	if (found) {
		if (found->prefer) {
			FREE_SAFE(found->prefer);
		}
		found->prefer = xstrdup(prefer);
	} else {
		return -1;
	}

	return 0;
}

int ntp_server_array_set_delete(UT_array *servers, char *name, bool delete_val)
{
	ntp_server_t find = {0}, *found = NULL;
	find.name = name;

	found = utarray_find(servers, &find, ntp_server_cmp_fn);
	if (found) {
		found->delete = delete_val;
	} else {
		return -1;
	}

	return 0;
}

void ntp_server_array_free(UT_array *servers)
{
	utarray_free(servers);
}

int save_ntp_config(UT_array *servers)
{
	char *cfg_entry = NULL;
	FILE *fp = NULL;
	FILE *fp_tmp = NULL;
	char *line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	int read_fd = -1;
	int write_fd = -1;
	struct stat stat_buf = {0};
	off_t offset = 0;
	ntp_server_t *iter = NULL;

	// open ntp.conf file for reading
	fp = fopen(NTP_CONFIG_FILE, "r");
	if (fp == NULL) {
		goto fail;
	}

	fp_tmp = fopen(NTP_TEMP_FILE, "a");
	if (fp_tmp == NULL) {
		goto fail;
	}

	// create a copy of ntp.conf file without ntp server entries
	while ((read = getline(&line, &len, fp)) != -1) {
		// if a line starts with server/pool/peer:
		if (strcmp(&line[0], "#") != 0 &&
			(strncmp(line, "server", strlen("server")) == 0 ||
			 strncmp(line, "peer", strlen("peer")) == 0 ||
			 strncmp(line, "pool", strlen("pool")) == 0)) {
			continue;
		} else {
			fputs(line, fp_tmp);
		}
	}
	FREE_SAFE(line);
	fclose(fp);

	// used for deletion of servers
	unsigned int idx = 0;
	while ((iter = (ntp_server_t *) utarray_next(servers, iter)) != NULL) {
		if (iter->name == NULL) {
			continue;
		}

		if (iter->delete == true) {
			// first step back for next iteration
			iter = utarray_prev(servers, iter);

			// delete the current server and move on
			utarray_erase(servers, idx, 1);
			--idx;
			continue;
		} else {
			cfg_entry = xmalloc(NTP_MAX_ENTRY_LEN);

			// construct the entry string
			snprintf(cfg_entry, NTP_MAX_ENTRY_LEN, "%s %s%c%s %s %s\n",
					 iter->assoc_type,
					 iter->address,
					 (iter->port == NULL) ? ' ' : ':',
					 (iter->port == NULL) ? "" : iter->port,
					 (iter->iburst == NULL) ? "" : iter->iburst,
					 (iter->prefer == NULL) ? "" : iter->prefer);

			// save it to the ntp config temp file
			fputs(cfg_entry, fp_tmp);

			FREE_SAFE(cfg_entry);
		}

		idx++;
	}

	fclose(fp_tmp);

	// create a backup file of /etc/ntp.conf
	if (rename(NTP_CONFIG_FILE, NTP_BAK_FILE) != 0) {
		goto fail;
	}

	// copy the temp file to /etc/ntp.conf
	read_fd = open(NTP_TEMP_FILE, O_RDONLY);
	if (read_fd == -1) {
		goto fail;
	}

	if (fstat(read_fd, &stat_buf) != 0) {
		goto fail;
	}

	write_fd = open(NTP_CONFIG_FILE, O_WRONLY | O_CREAT, stat_buf.st_mode);
	if (write_fd == -1) {
		goto fail;
	}

	if (sendfile(write_fd, read_fd, &offset, (size_t) stat_buf.st_size) == -1) {
		goto fail;
	}

	// remove the temp file
	if (remove(NTP_TEMP_FILE) != 0) {
		goto fail;
	}

	close(read_fd);
	close(write_fd);

	return 0;

fail:
	if (fp != NULL) {
		fclose(fp);
	}

	if (fp_tmp != NULL) {
		fclose(fp);
	}

	if (read_fd > 0) {
		close(read_fd);
	}

	if (write_fd > 0) {
		close(write_fd);
	}
	return -1;
}

int ntp_parse_config(ntp_server_t *server_entry, char *line)
{
	int error = 0;
	int count = 0;
	char *token = NULL;
	size_t tmp_len = 0;

	token = strtok(line, " ");
	if (token == NULL) {
		goto error_out;
	}

	while (token != NULL) {
		// maybe use a switch(count): case 1: case 2: etc.
		switch (count) {
			case 0: // association type
				tmp_len = strlen(line);

				server_entry->assoc_type = xstrndup(line, tmp_len + 1);
				break;
			case 1: // server address and port (if any)
				tmp_len = strlen(token);

				// check if port is present
				if (strstr(token, ":") != NULL) {
					// allocate engouh mem
					server_entry->address = xmalloc(NTP_MAX_ADDR_LEN + 1);
					server_entry->port = xmalloc(NTP_MAX_PORT_LEN + 1);

					// get the address and port
					error = sscanf(token, "%[^:]:%s", server_entry->address, server_entry->port);
					if (error == EOF) {
						goto error_out;
					}
				} else {
					// only address is present
					server_entry->address = xstrndup(token, tmp_len + 1);
				}

				// get name of server from file
				error = ntp_get_server_name(&server_entry->name, server_entry->address);
				if (error != 0) {
					goto error_out;
				}
				break;

			case 2: // "iburst" or "prefer"
				// check if "iburst" or "prefer"
				tmp_len = strlen(token);

				if (strncmp(token, "iburst", NTP_MAX_IBURST_LEN) == 0) {
					server_entry->iburst = xstrndup(token, tmp_len + 1);
				} else if (strncmp(token, "prefer", NTP_MAX_PREFER_LEN) == 0) {
					server_entry->prefer = xstrndup(token, tmp_len + 1);
				}
				break;

			case 3: // "iburst" or "prefer"
				// check if "iburst" or "prefer"
				tmp_len = strlen(token);
				if (strncmp(token, "iburst", NTP_MAX_IBURST_LEN) == 0) {
					server_entry->iburst = xstrndup(token, tmp_len + 1);
				} else if (strncmp(token, "prefer", NTP_MAX_PREFER_LEN) == 0) {
					server_entry->prefer = xstrndup(token, tmp_len + 1);
				}
				break;

			default:
				goto error_out;
		}

		token = strtok(NULL, " ");
		count++;
	}

	return 0;

error_out:

	return -1;
}

int ntp_server_list_add_server(ntp_server_list_t *sl, char *name)
{
	bool name_found = false;

	if (sl->count >= NTP_MAX_SERVERS) {
		return EINVAL;
	}

	for (int i = 0; i < sl->count; i++) {
		if (strcmp(sl->servers[i].name, name) == 0) {
			if (sl->servers[i].name != NULL) { // in case we deleted a server it will be NULL
				name_found = true;
				break;
			}
		}
	}

	if (!name_found) {
		// set the new server to the first free one in the list
		// the one with name == 0
		int pos = sl->count;
		for (int i = 0; i < sl->count; i++) {
			if (sl->servers[i].name == NULL) {
				pos = i;
				break;
			}
		}

		ntp_server_set_name(&sl->servers[pos], name);

		if (pos == sl->count) {
			++sl->count;
		}
	}

	return 0;
}

void ntp_server_copy_fn(void *dst, const void *src)
{
	ntp_server_t *d = (ntp_server_t *) dst;
	ntp_server_t *s = (ntp_server_t *) src;

	d->name = s->name ? xstrdup(s->name) : NULL;
	d->address = s->address ? xstrdup(s->address) : NULL;
	d->port = s->port ? xstrdup(s->port) : NULL;
	d->assoc_type = s->assoc_type ? xstrdup(s->assoc_type) : NULL;
	d->iburst = s->iburst ? xstrdup(s->iburst) : NULL;
	d->prefer = s->prefer ? xstrdup(s->prefer) : NULL;
	d->delete = s->delete;
}

void ntp_server_dtor_fn(void *elt)
{
	ntp_server_free(elt);
}

int ntp_server_cmp_fn(const void *p1, const void *p2)
{
	const ntp_server_t *s1 = p1;
	const ntp_server_t *s2 = p2;
	return strcmp(s1->name, s2->name);
}