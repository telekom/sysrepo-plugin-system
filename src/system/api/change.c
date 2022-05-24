#include "change.h"
#include "load.h"
#include "store.h"

#include <unistd.h>

#include <sysrepo.h>

int system_change_hostname_create(system_ctx_t *ctx, const char *value)
{
	int error = 0;

	error = system_store_hostname(ctx, value);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_store_hostname() error (%d)", error);
		return -1;
	}

	return 0;
}

int system_change_hostname_modify(system_ctx_t *ctx, const char *old_value, const char *new_value)
{
	int error = 0;

	error = system_store_hostname(ctx, new_value);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_store_hostname() error (%d)", error);
		return -1;
	}

	return 0;
}

int system_change_hostname_delete(system_ctx_t *ctx)
{
	int error = 0;

	error = system_store_hostname(ctx, "none");
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_store_hostname() error (%d)", error);
		return -1;
	}

	return 0;
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

int system_change_timezone_name_create(system_ctx_t *ctx, const char *value)
{
	int error = 0;

	error = system_store_timezone_name(ctx, value);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_store_timezone_name() error (%d)", error);
		return -1;
	}

	return 0;
}

int system_change_timezone_name_modify(system_ctx_t *ctx, const char *old_value, const char *new_value)
{
	(void) old_value;
	return system_change_timezone_name_create(ctx, new_value);
}

int system_change_timezone_name_delete(system_ctx_t *ctx)
{
	(void) ctx;

	int error = 0;

	error = access(SYSTEM_LOCALTIME_FILE, F_OK);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "/etc/localtime doesn't exist");
		goto error_out;
	}

	error = unlink(SYSTEM_LOCALTIME_FILE);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unlink() failed (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	return error;
}