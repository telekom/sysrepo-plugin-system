#include "load.h"

#include <unistd.h>

int system_load_hostname(system_ctx_t *ctx, char buffer[SYSTEM_HOSTNAME_LENGTH_MAX])
{
	int error = 0;

	error = gethostname(buffer, SYSTEM_HOSTNAME_LENGTH_MAX);
	if (error) {
		return -1;
	}

	return 0;
}

int system_load_contact(system_ctx_t *ctx, char buffer[256])
{
	int error = 0;
	return error;
}

int system_load_location(system_ctx_t *ctx, char buffer[256])
{
	int error = 0;
	return error;
}
