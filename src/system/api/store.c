#include "store.h"

#include <unistd.h>
#include <string.h>

int system_store_hostname(system_ctx_t *ctx, const char *hostname)
{
	int error = 0;

	const size_t len = strlen(hostname);

	error = sethostname(hostname, len);
	if (error) {
		return -1;
	}

	return 0;
}

int system_store_contact(system_ctx_t *ctx, const char *contact)
{
	int error = 0;

	return error;
}

int system_store_location(system_ctx_t *ctx, const char *location)
{
	int error = 0;

	return error;
}