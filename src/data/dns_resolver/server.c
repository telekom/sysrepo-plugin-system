#include "server.h"
#include "utils/memory.h"

void system_dns_server_init(system_dns_server_t *server)
{
	*server = (system_dns_server_t){0};
}

int system_dns_server_set_name(system_dns_server_t *server, const char *name)
{
	int error = 0;

	server->name = xstrdup(name);

	return error;
}

int system_dns_server_set_address(system_dns_server_t *server, system_ip_address_t address)
{
	int error = 0;
#ifdef SYSTEMD
	server->address = address;
#else
	server->address.value = xstrdup(address.value);
#endif

	return error;
}

int system_dns_server_set_port(system_dns_server_t *server, int port)
{
	int error = 0;

	server->port = port;

	return error;
}

void system_dns_server_free(system_dns_server_t *server)
{
	if (server->name) {
		free((void *) server->name);
	}

#ifndef SYSTEMD
	if (server->address.value) {
		free((void *) server->address.value);
	}
#endif

	system_dns_server_init(server);
}
