#include "array.h"
#include "types.h"
#include "common.h"
#include "system/ntp/data/server.h"

static void system_ntp_server_copy_fn(void *dst, const void *src);
static void system_ntp_server_dtor_fn(void *elt);

void system_ntp_server_array_init(UT_array **servers)
{
	*servers = NULL;

	UT_icd ntp_servers_icd = {
		sizeof(system_ntp_server_t),
		.init = NULL,
		.copy = system_ntp_server_copy_fn,
		.dtor = system_ntp_server_dtor_fn,
	};

	utarray_new(*servers, &ntp_servers_icd);
	utarray_reserve(*servers, SYSTEM_NTP_SERVER_MAX_COUNT);
}

void system_ntp_server_array_free(UT_array **servers)
{
	utarray_free(*servers);
	*servers = NULL;
}

static void system_ntp_server_copy_fn(void *dst, const void *src)
{
}

static void system_ntp_server_dtor_fn(void *elt)
{
	system_ntp_server_free((system_ntp_server_t *) elt);
}
