#include "ip_address.h"
#include "utils/memory.h"

#include <arpa/inet.h>

void system_ip_address_init(system_ip_address_t *address)
{
	*address = (system_ip_address_t){0};
}

int system_ip_address_to_str(system_ip_address_t *address, char *buffer, const unsigned int buffer_size)
{
#ifdef SYSTEMD
	switch (address->family) {
		case AF_INET:
			if (inet_ntop(AF_INET, address->value.v4, buffer, buffer_size) == NULL) {
				return -1;
			}
			return 0;
			break;
		case AF_INET6:
			if (inet_ntop(AF_INET6, address->value.v6, buffer, buffer_size) == NULL) {
				return -1;
			}
			break;
		default:
			break;
	}
#else
	if (snprintf(buffer, buffer_size, "%s", address->value) > 0) {
		return 0;
	}
#endif

	return -1;
}

void system_ip_address_free(system_ip_address_t *address)
{
#ifndef SYSTEMD
	if (address->value) {
		free((void *) address->value);
	}
#endif
	system_ip_address_init(address);
}

int system_ip_address_from_str(system_ip_address_t *address, const char *str)
{
	int error = 0;

#ifdef SYSTEMD
	if (inet_pton(AF_INET, str, address->value.v4) == 1) {
		address->family = AF_INET;
	} else if (inet_pton(AF_INET6, str, address->value.v6) == 1) {
		address->family = AF_INET;
	} else {
		// should not be possible -> yang model already checks this, but just in case return an error
		error = -1;
	}
#else
#endif

	return error;
}