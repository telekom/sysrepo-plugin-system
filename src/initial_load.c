#include "initial_load.h"
#include "common.h"
#include "utils/user_auth/user_authentication.h"
#include "utils/memory.h"

#include <errno.h>

// split initial load into parts
static int system_initial_load_local_users(system_ctx_t *ctx, sr_session_ctx_t *session);
static int system_initial_load_ntp_servers(system_ctx_t *ctx, sr_session_ctx_t *session);
static int system_initial_load_dns_servers(system_ctx_t *ctx, sr_session_ctx_t *session);

int system_initial_load(system_ctx_t *ctx, sr_session_ctx_t *session)
{
	int error = 0;
	char contact_info[MAX_GECOS_LEN] = {0};
	char hostname[HOST_NAME_MAX] = {0};
	char location[MAX_LOCATION_LENGTH] = {0};
	char *location_file_path = NULL;
	struct stat stat_buf = {0};

	// get the contact info from /etc/passwd
	error = system_get_contact_info(contact_info);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "get_contact_info error: %s", strerror(errno));
		goto error_out;
	}

	error = sr_set_item_str(session, CONTACT_YANG_PATH, contact_info, NULL, SR_EDIT_DEFAULT);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	// get the hostname of the system
	error = gethostname(hostname, HOST_NAME_MAX);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "gethostname error: %s", strerror(errno));
		goto error_out;
	}

	error = sr_set_item_str(session, HOSTNAME_YANG_PATH, hostname, NULL, SR_EDIT_DEFAULT);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	error = system_initial_load_local_users(ctx, session);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_initial_load_local_users() error (%d)", error);
		goto error_out;
	}

	error = system_initial_load_ntp_servers(ctx, session);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_initial_load_ntp_servers() error (%d)", error);
		goto error_out;
	}

	error = system_initial_load_dns_servers(ctx, session);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_initial_load_dns_servers() error (%d)", error);
		goto error_out;
	}

	// TODO: comment out for now because: "if-feature timezone-name;"
	//		 the feature has to be enabled in order to set the item
	/*
	char timezone_name[TIMEZONE_NAME_LEN] = {0};
	// get the current datetime (timezone-name) of the system
	error = get_timezone_name(timezone_name);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "get_timezone_name error: %s", strerror(errno));
		goto error_out;
	}

	error = sr_set_item_str(session, TIMEZONE_NAME_YANG_PATH, timezone_name, NULL, SR_EDIT_DEFAULT);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}
	*/

	// check if the location file is not empty
	location_file_path = system_get_plugin_file_path(LOCATION_FILENAME, false);
	if (location_file_path == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "get_plugin_file_path: couldn't get location file path");
		goto error_out;
	}

	error = stat(location_file_path, &stat_buf);
	if (error == -1) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "stat error (%d): %s", error, strerror(errno));
		goto error_out;
	}

	// if it's not empty, get the location
	if (stat_buf.st_size >= 1) {
		// get the location of the system
		error = system_get_location(location);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "get_location error: %s", strerror(errno));
			goto error_out;
		}

		error = sr_set_item_str(session, LOCATION_YANG_PATH, location, NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	error = sr_apply_changes(session, 0);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_apply_changes error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	FREE_SAFE(location_file_path);

	return 0;

error_out:
	if (location_file_path != NULL) {
		FREE_SAFE(location_file_path);
	}
	return -1;
}

int system_initial_load_local_users(system_ctx_t *ctx, sr_session_ctx_t *session)
{
	int error = 0;
	char tmp_buffer[PATH_MAX] = {0};

	local_user_t *user_iter = NULL;
	authorized_key_t *key_iter = NULL;

	while ((user_iter = utarray_next(ctx->local_users, user_iter)) != NULL) {
		// SRPLG_LOG_DBG(PLUGIN_NAME, "user name: %s", user_iter->name);
		error = snprintf(tmp_buffer, sizeof(tmp_buffer), "/ietf-system:system/authentication/user[name='%s']", user_iter->name);
		if (error < 0) {
			// snprintf error
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf failed");
			goto error_out;
		}
		error = sr_set_item_str(session, tmp_buffer, user_iter->name, NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		if (!user_iter->nologin) {
			// SRPLG_LOG_DBG(PLUGIN_NAME, "user password: %s", user_iter->password);
			error = snprintf(tmp_buffer, sizeof(tmp_buffer), "/ietf-system:system/authentication/user[name='%s']/password", user_iter->name);
			if (error < 0) {
				// snprintf error
				SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf failed");
				goto error_out;
			}
			error = sr_set_item_str(session, tmp_buffer, user_iter->password ? user_iter->password : "", NULL, SR_EDIT_DEFAULT);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error (%d): %s", error, sr_strerror(error));
				goto error_out;
			}
		}

		while ((key_iter = utarray_next(user_iter->auth_keys, key_iter)) != NULL) {
			// SRPLG_LOG_DBG(PLUGIN_NAME, "key: %s = %s", key_iter->name, key_iter->algorithm);
			error = snprintf(tmp_buffer, sizeof(tmp_buffer), "/ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']", user_iter->name, key_iter->name);
			if (error < 0) {
				// snprintf error
				SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf failed");
				goto error_out;
			}
			error = sr_set_item_str(session, tmp_buffer, key_iter->name, NULL, SR_EDIT_DEFAULT);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error (%d): %s", error, sr_strerror(error));
				goto error_out;
			}

			error = snprintf(tmp_buffer, sizeof(tmp_buffer), "/ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']/algorithm", user_iter->name, key_iter->name);
			if (error < 0) {
				// snprintf error
				SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf failed");
				goto error_out;
			}
			error = sr_set_item_str(session, tmp_buffer, key_iter->algorithm, NULL, SR_EDIT_DEFAULT);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error (%d): %s", error, sr_strerror(error));
				goto error_out;
			}

			// SRPLG_LOG_DBG(PLUGIN_NAME, "key: %s = %s", key_iter->name, key_iter->key_data);
			error = snprintf(tmp_buffer, sizeof(tmp_buffer), "/ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']/key-data", user_iter->name, key_iter->name);
			if (error < 0) {
				// snprintf error
				SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf failed");
				goto error_out;
			}
			error = sr_set_item_str(session, tmp_buffer, key_iter->key_data, NULL, SR_EDIT_DEFAULT);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error (%d): %s", error, sr_strerror(error));
				goto error_out;
			}
		}
	}

	goto out;

error_out:
	error = -1;

out:
	return error;
}

int system_initial_load_ntp_servers(system_ctx_t *ctx, sr_session_ctx_t *session)
{
	int error = 0;

	return error;
}

int system_initial_load_dns_servers(system_ctx_t *ctx, sr_session_ctx_t *session)
{
	int error = 0;

	return error;
}