#include "store.h"
#include "common.h"

#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sysrepo.h>
#include <unistd.h>
#include <utlist.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <shadow.h>

#include <srpc.h>
#include <umgmt.h>

static int system_authentication_user_create_home(const char *username, const uid_t uid, const gid_t gid);
static int system_authentication_user_copy_skel(const char *username);

int system_authentication_store_user(system_ctx_t *ctx, system_local_user_element_t *head)
{
	int error = -1;
	system_local_user_element_t *iter = NULL;
	um_user_db_t *user_db = NULL;
	um_user_t *new_user = NULL;
	char home_dir_buffer[PATH_MAX] = {0};
	bool user_added = false;

	user_db = um_user_db_new();
	if (!user_db) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "um_user_db_new() failed");
		goto error_out;
	}

	// load users
	error = um_user_db_load(user_db);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "um_user_db_load() error (%d)", error);
		goto error_out;
	}

	// add all users
	LL_FOREACH(head, iter)
	{
		const char *username = iter->user.name;
		const char *password = iter->user.password;

		// check if user already exists
		if (um_user_db_get_user(user_db, username)) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "User %s already exists in the database", username);
			goto error_out;
		}

		// create new user
		new_user = um_user_new();
		if (!new_user) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "um_user_new() failed");
			goto error_out;
		}
		user_added = false;

		// name
		error = um_user_set_name(new_user, username);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "um_user_set_name() error (%d)", error);
			goto error_out;
		}

		// password in passwd
		error = um_user_set_password(new_user, "x");
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "um_user_set_password() error (%d)", error);
			goto error_out;
		}

		// password in shadow
		error = um_user_set_password_hash(new_user, password);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "um_user_set_password_hash() error (%d)", error);
			goto error_out;
		}

		// gecos
		error = um_user_set_gecos(new_user, SYSTEM_AUTHENTICATION_DEFAULT_GECOS);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "um_user_set_gecos() error (%d)", error);
			goto error_out;
		}

		// default shell
		error = um_user_set_shell_path(new_user, SYSTEM_AUTHENTICATION_DEFAULT_SHELL);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "um_user_set_shell_path() error (%d)", error);
			goto error_out;
		}

		// home path
		error = snprintf(home_dir_buffer, sizeof(home_dir_buffer), "/home/%s", username);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error");
			goto error_out;
		}
		error = um_user_set_home_path(new_user, home_dir_buffer);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "um_user_set_shell_path() error (%d)", error);
			goto error_out;
		}

		// uid and gid
		um_user_set_uid(new_user, um_user_db_get_new_uid(user_db));
		um_user_set_gid(new_user, um_user_db_get_new_gid(user_db));

		// shadow data
		um_user_set_last_change(new_user, -1);
		um_user_set_change_min(new_user, 0);
		um_user_set_change_max(new_user, 99999);
		um_user_set_warn_days(new_user, 7);
		um_user_set_expiration(new_user, -1);
		um_user_set_inactive_days(new_user, -1);

		// add new user to the database
		error = um_user_db_add_user(user_db, new_user);
		user_added = true;
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "um_user_db_add_user() error (%d)", error);
			goto error_out;
		}
	}

	// store database data after all users have been added
	error = um_user_db_store(user_db);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "um_user_db_store() error (%d)", error);
		goto error_out;
	}

	// create home directories and copy /etc/skel data
	LL_FOREACH(head, iter)
	{
		const char *username = iter->user.name;
		const um_user_t *um_user = um_user_db_get_user(user_db, username);

		// the user has to be in the database since we've just added him in steps above
		assert(um_user != NULL);

		// get uid and gid for chown() when creating home directory
		const uid_t uid = um_user_get_uid(um_user);
		const gid_t gid = um_user_get_gid(um_user);

		// create home directory
		error = system_authentication_user_create_home(username, uid, gid);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_user_create_home() error (%d)", error);
			goto error_out;
		}

		// copy /etc/skel contents
		error = system_authentication_user_copy_skel(username);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_user_copy_skel() error (%d)", error);
			goto error_out;
		}
	}

	goto out;

error_out:
	error = -1;

out:
	if (!user_added && new_user) {
		um_user_free(new_user);
	}

	if (user_db) {
		um_user_db_free(user_db);
	}
	return error;
}

int system_authentication_store_user_authorized_key(system_ctx_t *ctx, const char *user, system_authorized_key_element_t *head)
{
	int error = -1;
	return error;
}

static int system_authentication_user_create_home(const char *username, const uid_t uid, const gid_t gid)
{
	int error = 0;
	char path_buffer[PATH_MAX] = {0};
	DIR *home_dir = NULL;

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

static int system_authentication_user_copy_skel(const char *username)
{
	int error = 0;
	char source_buffer[PATH_MAX] = {0};
	char destination_buffer[PATH_MAX] = {0};
	char home_path_buffer[PATH_MAX] = {0};
	char skel_path_buffer[PATH_MAX] = {0};

	DIR *dir = NULL;
	struct dirent *dir_entry = NULL;

	if (snprintf(skel_path_buffer, sizeof(skel_path_buffer), "%s", SYSTEM_AUTHENTICATION_SKEL_DIRECTORY) < 0) {
		goto error_out;
	}

	if (snprintf(home_path_buffer, sizeof(home_path_buffer), "/home/%s", username) < 0) {
		goto error_out;
	}

	if ((dir = opendir(skel_path_buffer)) == NULL) {
		SRPLG_LOG_INF(PLUGIN_NAME, "Unable to open directory %s", skel_path_buffer);
		goto error_out;
	} else {
		// copy all files from /etc/skel directory to the user home directory
		while ((dir_entry = readdir(dir)) != NULL) {
			if (strcmp(dir_entry->d_name, ".") && strcmp(dir_entry->d_name, "..") && dir_entry->d_type != DT_DIR) {
				if (snprintf(source_buffer, sizeof(source_buffer), "%s/%s", skel_path_buffer, dir_entry->d_name) < 0) {
					goto error_out;
				}

				if (snprintf(destination_buffer, sizeof(destination_buffer), "%s/%s", home_path_buffer, dir_entry->d_name) < 0) {
					goto error_out;
				}

				error = srpc_copy_file(source_buffer, destination_buffer);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_copy_file() error (%d) for %s --> %s", error, source_buffer, destination_buffer);
					goto error_out;
				}
			}
		}
	}

	goto out;

error_out:
	error = -1;

out:

	if (dir) {
		closedir(dir);
	}

	return error;
}