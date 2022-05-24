#include "store.h"
#include "common.h"

#include <unistd.h>
#include <string.h>

#include <sysrepo.h>

int system_store_hostname(system_ctx_t *ctx, const char *hostname)
{
	int error = 0;

	const size_t len = strlen(hostname);

	error = sethostname(hostname, len);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sethostname() failed");
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

int system_store_timezone_name(system_ctx_t *ctx, const char *timezone_name)
{
	int error = 0;
	char path_buffer[PATH_MAX] = {0};

	error = snprintf(path_buffer, sizeof(path_buffer), "%s/%s", SYSTEM_TIMEZONE_DIR, timezone_name);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error (%d)", error);
		goto error_out;
	}

	error = access(path_buffer, F_OK);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "access() failed (%d)", error);
		goto error_out;
	}

	if (access(SYSTEM_LOCALTIME_FILE, F_OK) == 0) {
		error = unlink(SYSTEM_LOCALTIME_FILE);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "unlink() failed (%d)", error);
			goto error_out;
		}
	}

	error = symlink(path_buffer, SYSTEM_LOCALTIME_FILE);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "symlink() failed (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:

	return error;
}