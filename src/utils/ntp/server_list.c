#include "server_list.h"
#include "../memory.h"
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

void ntp_server_init(ntp_server_t *s)
{
	s->name = NULL;
	s->address= NULL;
	s->port = NULL;
	s->assoc_type = NULL;
	s->iburst = NULL;
	s->prefer = NULL;
	s->delete = false;
}

void ntp_server_set_name(ntp_server_t *s, char *name)
{
	unsigned long tmp_len = 0;
	tmp_len = strlen(name);
	s->name = xstrndup(name, tmp_len+1);
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

int ntp_server_list_init(sr_session_ctx_t *session, ntp_server_list_t **sl)
{
	int error = 0;

	*sl = xmalloc(sizeof(ntp_server_list_t));

	for (int i = 0; i < NTP_MAX_SERVERS; i++) {
		ntp_server_init(&(*sl)->servers[i]);
	}
	(*sl)->count = 0;

	// add existing ntp servers to internal list
	error = ntp_server_list_add_existing_servers(session, *sl);
	if (error != 0) {
		return -1;
	}

	return 0;
}

int ntp_server_list_add_existing_servers(sr_session_ctx_t *session, ntp_server_list_t *sl)
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
			error = ntp_add_server_entry_to_list(sl, &server_entry);
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
		switch (count){
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

int ntp_add_server_entry_to_list(ntp_server_list_t *sl, ntp_server_t *server_entry)
{
	int error = 0;

	error = ntp_server_list_add_server(sl, server_entry->name);
	if (error != 0) {
		return -1;
	}

	error = ntp_server_list_set_address(sl, server_entry->name, server_entry->address);
	if (error != 0) {
		return -1;
	}

	if (server_entry->port != NULL) {
		error = ntp_server_list_set_port(sl, server_entry->name, server_entry->port);
		if (error != 0) {
			return -1;
		}
	}

	error = ntp_server_list_set_assoc_type(sl, server_entry->name, server_entry->assoc_type);
	if (error != 0) {
		return -1;
	}

	if (server_entry->iburst != NULL) {
		error = ntp_server_list_set_iburst(sl, server_entry->name, server_entry->iburst);
		if (error != 0) {
			return -1;
		}
	}

	if (server_entry->prefer != NULL) {
		error = ntp_server_list_set_prefer(sl, server_entry->name, server_entry->prefer);
		if (error != 0) {
			return -1;
		}
	}

	return 0;
}

int ntp_server_list_add_server(ntp_server_list_t *sl, char *name)
{
	bool name_found = false;

	if (sl->count >= NTP_MAX_SERVERS) {
		return EINVAL;
	}

	for (int i = 0; i < sl->count; i++) {
		if (sl->servers[i].name != NULL) { // in case we deleted a server it will be NULL
			if (strcmp(sl->servers[i].name, name) == 0) {
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

int ntp_server_list_set_address(ntp_server_list_t *sl, char *name, char *address)
{
	bool server_found = false;

	for (int i = 0; i < sl->count; i++) {
		if (sl->servers[i].name != NULL) {
			if (strcmp(sl->servers[i].name, name) == 0) {
				// if the address was already allocated (set)
				// free it first
				if (sl->servers[i].address != NULL) {
					FREE_SAFE(sl->servers[i].address);
				}

				unsigned long tmp_len = 0;
				tmp_len = strlen(address);
				sl->servers[i].address = xstrndup(address, tmp_len+1);

				server_found = true;
				break;
			}
		}
	}
	if (!server_found) {
		return -1;
	}
	return 0;
}

int ntp_server_list_set_port(ntp_server_list_t *sl, char *name, char *port)
{
	bool server_found = false;

	for (int i = 0; i < sl->count; i++) {
		if (sl->servers[i].name != NULL) {
			if (strcmp(sl->servers[i].name, name) == 0) {
				// if the port was already allocated (set)
				// free it first
				if (sl->servers[i].port != NULL) {
					FREE_SAFE(sl->servers[i].port);
				}
				unsigned long tmp_len = 0;
				tmp_len = strlen(port);
				sl->servers[i].port = xstrndup(port, tmp_len+1);

				server_found = true;
				break;
			}
		}
	}
	if (!server_found) {
		return -1;
	}
	return 0;
}

int ntp_server_list_set_assoc_type(ntp_server_list_t *sl, char *name, char *assoc_type)
{
	bool server_found = false;

	for (int i = 0; i < sl->count; i++) {
		if (sl->servers[i].name != NULL) {
			if (strcmp(sl->servers[i].name, name) == 0) {
				// if the assoc_type was already allocated (set)
				// free it first
				if (sl->servers[i].assoc_type != NULL) {
					FREE_SAFE(sl->servers[i].assoc_type);
				}
				unsigned long tmp_len = 0;
				tmp_len = strlen(assoc_type);
				sl->servers[i].assoc_type = xstrndup(assoc_type, tmp_len+1);

				server_found = true;
				break;
			}
		}
	}
	if (!server_found) {
		return -1;
	}
	return 0;
}

int ntp_server_list_set_iburst(ntp_server_list_t *sl, char *name, char *iburst)
{
	bool server_found = false;

	for (int i = 0; i < sl->count; i++) {
		if (sl->servers[i].name != NULL) {
			if (strcmp(sl->servers[i].name, name) == 0) {
				// if the iburst was already allocated (set)
				// free it first
				if (sl->servers[i].iburst != NULL) {
					FREE_SAFE(sl->servers[i].iburst);
				}
				unsigned long tmp_len = 0;
				tmp_len = strlen(iburst);
				sl->servers[i].iburst = xstrndup(iburst, tmp_len+1);

				server_found = true;
				break;
			}
		}
	}
	if (!server_found) {
		return -1;
	}
	return 0;
}

int ntp_server_list_set_prefer(ntp_server_list_t *sl, char *name, char *prefer)
{
	bool server_found = false;

	for (int i = 0; i < sl->count; i++) {
		if (sl->servers[i].name != NULL) {
			if (strcmp(sl->servers[i].name, name) == 0) {
				// if the prefer was already allocated (set)
				// free it first
				if (sl->servers[i].prefer != NULL) {
					FREE_SAFE(sl->servers[i].prefer);
				}
				unsigned long tmp_len = 0;
				tmp_len = strlen(prefer);
				sl->servers[i].prefer = xstrndup(prefer, tmp_len+1);

				server_found = true;
				break;
			}
		}
	}
	if (!server_found) {
		return -1;
	}
	return 0;
}

int ntp_server_list_set_delete(ntp_server_list_t *sl, char *name, bool delete_val)
{
	bool server_found = false;

	for (int i = 0; i < sl->count; i++) {
		if (sl->servers[i].name != NULL) {
			if (strcmp(sl->servers[i].name, name) == 0) {

				sl->servers[i].delete = delete_val;

				server_found = true;
				break;
			}
		}
	}
	if (!server_found) {
		return -1;
	}
	return 0;
}

// TODO: refactor this function; break it into smaller pieces etc.
int save_ntp_config(ntp_server_list_t *sl)
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

	// open ntp.conf file for reading
	fp = fopen(NTP_CONFIG_FILE, "r");
	if (fp == NULL) {
		goto fail;
	}

	fp_tmp = fopen (NTP_TEMP_FILE, "a");
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

	// save all ntp servers from ntp_servers list
	for (int i = 0; i < sl->count; i++) {
		if (sl->servers[i].name == NULL) {
			continue;
		}

		if (sl->servers[i].delete == true) {
			ntp_server_free(&sl->servers[i]);
		} else {
			cfg_entry = xmalloc(NTP_MAX_ENTRY_LEN);

			// construct the entry string
			snprintf(cfg_entry, NTP_MAX_ENTRY_LEN, "%s %s%c%s %s %s\n",
					sl->servers[i].assoc_type,
					sl->servers[i].address,
					(sl->servers[i].port == NULL) ? ' ' : ':',
					(sl->servers[i].port == NULL) ? "" : sl->servers[i].port,
					(sl->servers[i].iburst == NULL) ? "" : sl->servers[i].iburst,
					(sl->servers[i].prefer == NULL) ? "" : sl->servers[i].prefer);

			// save it to the ntp config temp file
			fputs(cfg_entry, fp_tmp);

			FREE_SAFE(cfg_entry);
		}
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

	if (sendfile(write_fd, read_fd, &offset, (size_t)stat_buf.st_size) == -1) {
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

void ntp_server_list_free(ntp_server_list_t *sl)
{
	for (int i = 0; i < sl->count; i++) {
		ntp_server_free(&sl->servers[i]);
	}
}