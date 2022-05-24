#include "change.h"
#include "load.h"
#include "store.h"

int system_change_hostname_create(system_ctx_t *ctx, const char *value)
{
	return system_store_hostname(ctx, value);
}

int system_change_hostname_modify(system_ctx_t *ctx, const char *old_value, const char *new_value)
{
	return system_store_hostname(ctx, new_value);
}

int system_change_hostname_delete(system_ctx_t *ctx)
{
	return system_store_hostname(ctx, "none");
}

int system_change_contact_create(system_ctx_t *ctx, const char *value)
{
	int error = 0;

	return error;
}

int system_change_contact_modify(system_ctx_t *ctx, const char *old_value, const char *new_value)
{
	int error = 0;

	return error;
}

int system_change_contact_delete(system_ctx_t *ctx)
{
	int error = 0;

	return error;
}

int system_change_location_create(system_ctx_t *ctx, const char *value)
{
	int error = 0;

	return error;
}

int system_change_location_modify(system_ctx_t *ctx, const char *old_value, const char *new_value)
{
	int error = 0;

	return error;
}

int system_change_location_delete(system_ctx_t *ctx)
{
	int error = 0;

	return error;
}