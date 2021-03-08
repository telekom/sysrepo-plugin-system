#include "server_list.h"
#include "../memory.h"
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#define NTP_MAX_SERVERS 20 // TODO: update if needed
#define NTP_CONFIG_FILE "/etc/ntp.conf"
#define NTP_TEMP_FILE "/tmp/tmp_ntp.conf"
#define NTP_BAK_FILE "/etc/ntp.conf.bak"

typedef struct {
	char *name;
	char *address;
	char *port;
	char *assoc_type;
	char *iburst;
	char *prefer;
} ntp_server_t;

struct ntp_server_list_s {
	ntp_server_t servers[NTP_MAX_SERVERS];
	uint8_t count;
};

void ntp_server_init(ntp_server_t *s)
{
	s->name = xmalloc(sizeof(char));
	s->name[0] = 0;
	s->address= xmalloc(sizeof(char));
	s->address[0] = 0;
	s->port= xmalloc(sizeof(char));
	s->port[0] = 0;
	s->assoc_type = xmalloc(sizeof(char));
	s->assoc_type[0] = 0;
	s->iburst = xmalloc(sizeof(char));
	s->iburst[0] = 0;
	s->prefer = xmalloc(sizeof(char));
	s->prefer[0] = 0;
}

void ntp_server_set_name(ntp_server_t *s, char *name)
{
	unsigned long tmp_len = 0;
	tmp_len = strlen(name);
	s->name = xmalloc(sizeof(char) * (tmp_len + 1));
	memcpy(s->name, name, tmp_len);
	s->name[tmp_len] = 0;
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
}

void ntp_server_list_init(ntp_server_list_t **sl)
{
	*sl = xmalloc(sizeof(ntp_server_list_t));

	for (int i = 0; i < NTP_MAX_SERVERS; i++) {
		ntp_server_init(&(*sl)->servers[i]);
	}
	(*sl)->count = 0;
}

int ntp_server_list_add_server(ntp_server_list_t *sl, char *name)
{
	int error = 0;

	if (sl->count >= NTP_MAX_SERVERS) {
		error = EINVAL;
	} else {
		ntp_server_set_name(&sl->servers[sl->count], name);
		++sl->count;
	}
	return error;
}

int ntp_server_list_set_address(ntp_server_list_t *sl, char *name, char *address)
{
	bool server_found = false;

	for (int i = 0; i < sl->count; i++) {
		if (strcmp(sl->servers[i].name, name) == 0) {
			// if the address was already allocated (set)
			// free it first
			if (sl->servers[i].address != NULL) {
				FREE_SAFE(sl->servers[i].address);
			}

			unsigned long tmp_len = 0;
			tmp_len = strlen(address);
			sl->servers[i].address = xmalloc(sizeof(char) * (tmp_len + 1));
			memcpy(sl->servers[i].address, address, tmp_len);
			sl->servers[i].address[tmp_len] = 0;

			server_found = true;
			break;
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
		if (strcmp(sl->servers[i].name, name) == 0) {
			// if the port was already allocated (set)
			// free it first
			if (sl->servers[i].port != NULL) {
				FREE_SAFE(sl->servers[i].port);
			}
			unsigned long tmp_len = 0;
			tmp_len = strlen(port);
			sl->servers[i].port = xmalloc(sizeof(char) * (tmp_len + 1));
			memcpy(sl->servers[i].port, port, tmp_len);
			sl->servers[i].port[tmp_len] = 0;

			server_found = true;
			break;
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
		if (strcmp(sl->servers[i].name, name) == 0) {
			// if the assoc_type was already allocated (set)
			// free it first
			if (sl->servers[i].assoc_type != NULL) {
				FREE_SAFE(sl->servers[i].assoc_type);
			}
			unsigned long tmp_len = 0;
			tmp_len = strlen(assoc_type);
			sl->servers[i].assoc_type = xmalloc(sizeof(char) * (tmp_len + 1));
			memcpy(sl->servers[i].assoc_type, assoc_type, tmp_len);
			sl->servers[i].assoc_type[tmp_len] = 0;

			server_found = true;
			break;
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
		if (strcmp(sl->servers[i].name, name) == 0) {
			// if the iburst was already allocated (set)
			// free it first
			if (sl->servers[i].iburst != NULL) {
				FREE_SAFE(sl->servers[i].iburst);
			}
			unsigned long tmp_len = 0;
			tmp_len = strlen(iburst);
			sl->servers[i].iburst = xmalloc(sizeof(char) * (tmp_len + 1));
			memcpy(sl->servers[i].iburst, iburst, tmp_len);
			sl->servers[i].iburst[tmp_len] = 0;

			server_found = true;
			break;
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
		if (strcmp(sl->servers[i].name, name) == 0) {
			// if the prefer was already allocated (set)
			// free it first
			if (sl->servers[i].prefer != NULL) {
				FREE_SAFE(sl->servers[i].prefer);
			}
			unsigned long tmp_len = 0;
			tmp_len = strlen(prefer);
			sl->servers[i].prefer = xmalloc(sizeof(char) * (tmp_len + 1));
			memcpy(sl->servers[i].prefer, prefer, tmp_len);
			sl->servers[i].prefer[tmp_len] = 0;

			server_found = true;
			break;
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
	size_t entry_len = 0;
	size_t assoc_len = 0;
	size_t addr_len = 0;
	size_t port_len = 0;
	size_t iburst_len = 0;
	size_t prefer_len = 0;
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
		assoc_len = strlen(sl->servers[i].assoc_type);
		addr_len = strlen(sl->servers[i].address);
		port_len = strlen(sl->servers[i].port);
		iburst_len = strlen(sl->servers[i].iburst);
		prefer_len = strlen(sl->servers[i].prefer);

		entry_len = assoc_len + addr_len + port_len + iburst_len + prefer_len + 6; // +6 for 4 spaces, a newline char and \0

		cfg_entry = xmalloc(entry_len);

		// construct the entry string
		snprintf(cfg_entry, entry_len, "%s %s%c%s %s %s\n",
				sl->servers[i].assoc_type,
				sl->servers[i].address,
				strcmp(sl->servers[i].port,"")==0 ? ' ' : ':',
				sl->servers[i].port, sl->servers[i].iburst,
				sl->servers[i].prefer);

		// save it to the ntp config temp file
		fputs(cfg_entry, fp_tmp);

		FREE_SAFE(cfg_entry);
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
