#include "store.h"
#include "common.h"
#include "types.h"

#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sysrepo.h>
#include <unistd.h>
#include <utlist.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <shadow.h>
#include <srpc.h>

static int system_authentication_user_update_passwd(const char *username);
static int system_authentication_user_update_shadow(const char *username, const char *password);
static int system_authentication_user_create_home(const char *username);
static int system_authentication_user_get_uid_and_gid(const char *username, uid_t *uid, gid_t *gid);
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

		// 4. copy files from /etc/skel - .bashrc, .profile etc. to home directory
		error = system_authentication_user_copy_skel(iter->user.name);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_user_copy_skel() error (%d)", error);
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
	FILE *backup_pwd_file = NULL;
	struct passwd *pwd = NULL;
	struct passwd new_entry = {0};
	uid_t uid = 0;
	gid_t gid = 0;
	char home_dir_buffer[PATH_MAX] = {0};
	bool db_closed = false;

	backup_pwd_file = fopen(SYSTEM_AUTHENTICATION_PASSWD_TEMP_PATH, "w");
	if (!backup_pwd_file) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to create temporary passwd file");
		goto error_out;
	}

	// copy current database to temp file
	setpwent();

	while ((pwd = getpwent()) != NULL) {
		error = putpwent(pwd, backup_pwd_file);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "putpwent() error (%d)", error);
			goto error_out;
		}

		if (pwd->pw_uid >= 1000) {
			uid = pwd->pw_uid;
			gid = pwd->pw_gid;
		}
	}

	// setup new user entry

	// name
	new_entry.pw_name = strdup(username);
	if (!new_entry.pw_name) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "strdup() failed for new entry name");
		goto error_out;
	}

	// password
	new_entry.pw_passwd = strdup("x");
	if (!new_entry.pw_name) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "strdup() failed for new entry passwd");
		goto error_out;
	}

	// shell
	new_entry.pw_shell = strdup(SYSTEM_AUTHENTICATION_DEFAULT_SHELL);
	if (!new_entry.pw_name) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "strdup() failed for new entry shell");
		goto error_out;
	}

	// uid and gid
	new_entry.pw_uid = uid + 1;
	new_entry.pw_gid = gid + 1;

	// home dir (to be created yet)
	error = snprintf(home_dir_buffer, sizeof(home_dir_buffer), "/home/%s", username);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed");
		goto error_out;
	}
	new_entry.pw_dir = strdup(home_dir_buffer);
	if (!new_entry.pw_dir) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "strdup() failed for new entry dir");
		goto error_out;
	}

	// add new entry
	error = putpwent(&new_entry, backup_pwd_file);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to add new entry to the users passwd database - putpwent() error (%d)", error);
		goto error_out;
	}

	// close database and temp file
	endpwent();
	fclose(backup_pwd_file);
	db_closed = true;
	backup_pwd_file = NULL;

	// rename /etc/passwd to old version
	error = rename(SYSTEM_AUTHENTICATION_PASSWD_PATH, SYSTEM_AUTHENTICATION_PASSWD_OLD_PATH);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rename() error (%d)", error);
		goto error_out;
	}

	// move temp file to /etc/passwd
	error = srpc_copy_file(SYSTEM_AUTHENTICATION_PASSWD_TEMP_PATH, SYSTEM_AUTHENTICATION_PASSWD_PATH);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_copy_file() error (%d)", error);
		goto error_out;
	}

	// remove temporary and old files
	error = remove(SYSTEM_AUTHENTICATION_PASSWD_TEMP_PATH);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "remove() failed for %s (%d)", SYSTEM_AUTHENTICATION_PASSWD_TEMP_PATH, error);
		goto error_out;
	}
	error = remove(SYSTEM_AUTHENTICATION_PASSWD_OLD_PATH);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "remove() failed for %s (%d)", SYSTEM_AUTHENTICATION_PASSWD_OLD_PATH, error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	// close DB if not already closed
	if (!db_closed) {
		endpwent();
	}

	// free new entry
	if (new_entry.pw_name) {
		free(new_entry.pw_name);
	}
	if (new_entry.pw_passwd) {
		free(new_entry.pw_passwd);
	}
	if (new_entry.pw_shell) {
		free(new_entry.pw_shell);
	}
	if (new_entry.pw_dir) {
		free(new_entry.pw_dir);
	}

	// close temp file
	if (backup_pwd_file) {
		fclose(backup_pwd_file);
	}

	return error;
}

static int system_authentication_user_update_shadow(const char *username, const char *password)
{
	int error = 0;
	FILE *backup_pwd_file = NULL;
	struct spwd *shd = NULL;
	struct spwd new_entry = {0};
	bool db_closed = false;

	backup_pwd_file = fopen(SYSTEM_AUTHENTICATION_SHADOW_TEMP_PATH, "w");
	if (!backup_pwd_file) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to create temporary shadow file");
		goto error_out;
	}

	// copy current database to temp file
	setspent();

	while ((shd = getspent()) != NULL) {
		error = putspent(shd, backup_pwd_file);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "putspent() error (%d)", error);
			goto error_out;
		}
	}

	// setup new user entry

	// name
	new_entry.sp_namp = strdup(username);
	if (!new_entry.sp_namp) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "strdup() failed for new entry name");
		goto error_out;
	}

	// password
	new_entry.sp_pwdp = strdup(password);
	if (!new_entry.sp_pwdp) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "strdup() failed for new entry passwd");
		goto error_out;
	}

	// last password change
	new_entry.sp_lstchg = -1;

	// validity
	new_entry.sp_max = 99999;

	// min # of days after which the password can be changed
	new_entry.sp_min = 0;

	// warning period
	new_entry.sp_warn = 7;

	// other fields
	new_entry.sp_expire = -1;
	new_entry.sp_inact = -1;

	// add new entry
	error = putspent(&new_entry, backup_pwd_file);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to add new entry to the shadow database - putspent() error (%d)", error);
		goto error_out;
	}

	// close database and temp file
	endspent();
	fclose(backup_pwd_file);
	db_closed = true;
	backup_pwd_file = NULL;

	// rename /etc/shadow to old version
	error = rename(SYSTEM_AUTHENTICATION_SHADOW_PATH, SYSTEM_AUTHENTICATION_SHADOW_OLD_PATH);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "rename() error (%d)", error);
		goto error_out;
	}

	// move temp file to /etc/shadow
	error = srpc_copy_file(SYSTEM_AUTHENTICATION_SHADOW_TEMP_PATH, SYSTEM_AUTHENTICATION_SHADOW_PATH);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_copy_file() error (%d)", error);
		goto error_out;
	}

	// remove temporary and old files
	error = remove(SYSTEM_AUTHENTICATION_SHADOW_TEMP_PATH);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "remove() failed for %s (%d)", SYSTEM_AUTHENTICATION_SHADOW_TEMP_PATH, error);
		goto error_out;
	}
	error = remove(SYSTEM_AUTHENTICATION_SHADOW_OLD_PATH);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "remove() failed for %s (%d)", SYSTEM_AUTHENTICATION_SHADOW_OLD_PATH, error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	// close DB if not already closed
	if (!db_closed) {
		endspent();
	}

	// free new entry
	if (new_entry.sp_namp) {
		free(new_entry.sp_namp);
	}
	if (new_entry.sp_pwdp) {
		free(new_entry.sp_pwdp);
	}

	// close temp file
	if (backup_pwd_file) {
		fclose(backup_pwd_file);
	}

	return error;
}

static int system_authentication_user_create_home(const char *username)
{
	int error = 0;
	char path_buffer[PATH_MAX] = {0};
	DIR *home_dir = NULL;
	uid_t uid = 0;
	gid_t gid = 0;

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

static int system_authentication_user_get_uid_and_gid(const char *username, uid_t *uid, gid_t *gid)
{
	int error = 0;
	bool found = false;
	struct passwd *pwd = {0};

	setpwent();

	while ((pwd = getpwent()) != NULL) {
		if (!strcmp(pwd->pw_name, username)) {
			found = true;
			*uid = pwd->pw_uid;
			*gid = pwd->pw_gid;
		}
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