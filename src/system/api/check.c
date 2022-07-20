#include "check.h"
#include "load.h"

#include <sysrepo.h>

srpc_check_status_t system_check_hostname(system_ctx_t *ctx, const char *hostname)
{
	srpc_check_status_t status = srpc_check_status_none;
	char hostname_buffer[SYSTEM_HOSTNAME_LENGTH_MAX] = {0};
	int error = 0;

	error = system_load_hostname(ctx, hostname_buffer);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_load_hostname() error (%d)", error);
		goto error_out;
	}

	error = strcmp(hostname, hostname_buffer);
	if (error == 0) {
		status = srpc_check_status_equal;
	} else {
		status = srpc_check_status_non_existant;
	}

	goto out;

error_out:
	status = srpc_check_status_error;

out:
	return status;
}

srpc_check_status_t system_check_contact(system_ctx_t *ctx, const char *contact)
{
	srpc_check_status_t status = srpc_check_status_none;

	return status;
}

srpc_check_status_t system_check_location(system_ctx_t *ctx, const char *location)
{
	srpc_check_status_t status = srpc_check_status_none;

	return status;
}

srpc_check_status_t system_check_timezone_name(system_ctx_t *ctx, const char *timezone_name)
{
	srpc_check_status_t status = srpc_check_status_none;
	char timezone_name_buffer[SYSTEM_TIMEZONE_NAME_LENGTH_MAX] = {0};
	int error = 0;

	error = system_load_timezone_name(ctx, timezone_name_buffer);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_load_timezone_name() error (%d)", error);
		goto error_out;
	}

	error = strcmp(timezone_name, timezone_name_buffer);
	if (error == 0) {
		status = srpc_check_status_equal;
	} else {
		status = srpc_check_status_non_existant;
	}

	goto out;

error_out:
	status = srpc_check_status_error;

out:
	return status;
}