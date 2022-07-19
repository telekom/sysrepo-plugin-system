#include "store.h"
#include "common.h"

#include <unistd.h>
#include <string.h>

#include <sysrepo.h>

int system_store_hostname(system_ctx_t *ctx, const char *hostname)
{
	int error = 0;

#ifdef AUGYANG
	int augeas = 0;
#endif

	const size_t len = strlen(hostname);

	error = sethostname(hostname, len);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sethostname() failed");
		return -1;
	}

#ifdef AUGYANG
	SRPLG_LOG_INF(PLUGIN_NAME, "Setting /etc/hostname value using augeas datastore plugin");
	error = sr_set_item_str(ctx->startup_session, "/hostname:hostname[config-file=\'/etc/hostname\']/hostname", hostname, NULL, 0);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str() error (%d): %s", error, sr_strerror(error));
	} else {
		SRPLG_LOG_INF(PLUGIN_NAME, "/etc/hostname set");
		augeas = 1;
	}

	if (augeas) {
		SRPLG_LOG_INF(PLUGIN_NAME, "Applying /etc/hostname changes");
		error = sr_apply_changes(ctx->startup_session, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_apply_changes() error (%d): %s", error, sr_strerror(error));
			return -1;
		}
	}
#endif

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