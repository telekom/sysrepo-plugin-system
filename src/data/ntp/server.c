#include "server.h"
#include "utils/memory.h"

void system_ntp_server_init(system_ntp_server_t *server)
{
	*server = (system_ntp_server_t){0};
}

int system_ntp_server_set_name(system_ntp_server_t *server, const char *name)
{
	int error = 0;

	server->name = xstrdup(name);

	return error;
}

int system_ntp_server_set_address(system_ntp_server_t *server, const char *address)
{
	int error = 0;

	server->address = xstrdup(address);

	return error;
}

int system_ntp_server_set_port(system_ntp_server_t *server, int port)
{
	int error = 0;

	server->port = port;

	return error;
}

int system_ntp_server_set_association_type(system_ntp_server_t *server, const char *association_type)
{
	int error = 0;

	server->association_type = xstrdup(association_type);

	return error;
}

int system_ntp_server_set_iburst(system_ntp_server_t *server, const char *iburst)
{
	int error = 0;

	server->iburst = xstrdup(iburst);

	return error;
}

int system_ntp_server_set_prefer(system_ntp_server_t *server, const char *prefer)
{
	int error = 0;

	server->iburst = xstrdup(prefer);

	return error;
}

void system_ntp_server_free(system_ntp_server_t *server)
{
	if (server->name) {
		free((void *) server->name);
	}

	if (server->address) {
		free((void *) server->address);
	}

	if (server->association_type) {
		free((void *) server->association_type);
	}

	if (server->iburst) {
		free((void *) server->iburst);
	}

	if (server->prefer) {
		free((void *) server->prefer);
	}

	system_ntp_server_init(server);
}