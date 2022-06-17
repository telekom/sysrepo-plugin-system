#include "store.h"
#include "common.h"
#include "types.h"

#include <stdbool.h>
#include <stdio.h>
#include <sysrepo.h>
#include <unistd.h>
#include <utlist.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>

static int system_authentication_user_update_passwd(const char *username);
static int system_authentication_user_update_shadow(const char *username, const char *password);
static int system_authentication_user_create_home(const char *username);
static int system_authentication_user_get_uid_and_gid(const char *username, __uid_t *uid, __gid_t *gid);
static int system_authentication_user_copy_skel(const char *username);

int system_authentication_store_user(system_ctx_t *ctx, system_local_user_element_t *head)
{
	int error = -1;
	system_local_user_element_t *iter = NULL;

	LL_FOREACH(head, iter)
	{
		// 1. add user to the passwd file
		error = system_authentication_user_update_passwd(iter->user.name);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_user_update_passwd() error (%d)", error);
			goto error_out;
		}

		// 2. add user to the shadow file
		error = system_authentication_user_update_shadow(iter->user.name, iter->user.password);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_user_update_shadow() error (%d)", error);
			goto error_out;
		}

		// 3. create home directory
		error = system_authentication_user_create_home(iter->user.name);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_user_create_home() error (%d)", error);
			goto error_out;
		}
	}

	goto out;

error_out:
	error = -1;

out:

	return error;
}

int system_authentication_store_user_authorized_key(system_ctx_t *ctx, const char *user, system_authorized_key_element_t *head)
{
	int error = -1;
	return error;
}

static int system_authentication_user_update_passwd(const char *username)
{
	int error = 0;
	return error;
}

static int system_authentication_user_update_shadow(const char *username, const char *password)
{
	int error = 0;
	return error;
}

static int system_authentication_user_create_home(const char *username)
{
	int error = 0;
	char path_buffer[PATH_MAX] = {0};
	DIR *home_dir = NULL;
	__uid_t uid = 0;
	__gid_t gid = 0;

	error = snprintf(path_buffer, sizeof(path_buffer), "/home/%s", username);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed");
		goto error_out;
	}

	home_dir = opendir(path_buffer);
	if (home_dir) {
		// home dir exists - unable to create it
		SRPLG_LOG_ERR(PLUGIN_NAME, "Home directory for user %s exists", username);
		goto error_out;
	} else if (errno == ENOENT) {
		// create directory

		// get uid and gid
		error = system_authentication_user_get_uid_and_gid(username, &uid, &gid);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_user_get_uid_and_gid() error (%d)", error);
			goto error_out;
		}

		// create dir
		error = mkdir(path_buffer, 0700);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "mkdir() error (%d)", error);
			goto error_out;
		}

		// set permissions to the directory
		error = chown(path_buffer, uid, gid);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "chown() error (%d)", error);
			goto error_out;
		}
	} else {
		SRPLG_LOG_ERR(PLUGIN_NAME, "opendir() failed");
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	if (home_dir) {
		closedir(home_dir);
	}

	return error;
}

static int system_authentication_user_get_uid_and_gid(const char *username, __uid_t *uid, __gid_t *gid)
{
	int error = 0;
	bool found = false;
	struct passwd *pwd = {0};

	setpwent();

	pwd = getpwent();
	if (!pwd) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "getpwent() failed");
		goto error_out;
	}

	while (pwd) {
		if (!strcmp(pwd->pw_name, username)) {
			found = true;
			*uid = pwd->pw_uid;
			*gid = pwd->pw_gid;
		}

		// continue
		pwd = getpwent();
	}

	if (!found) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "No user %s found in the passwd database", username);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	endpwent();

	return error;
}

static int system_authentication_user_copy_skel(const char *username)
{
	int error = 0;
	return error;
}