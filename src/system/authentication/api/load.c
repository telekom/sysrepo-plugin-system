#include "load.h"
#include "sysrepo.h"
#include "types.h"
#include "common.h"

#include "system/authentication/data/authorized_key/array.h"
#include "system/authentication/data/authorized_key.h"
#include "system/authentication/data/local_user/array.h"
#include "system/authentication/data/local_user.h"

#include <unistd.h>
#include <pwd.h>
#include <shadow.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <linux/limits.h>

static int system_check_file_extension(const char *path, const char *ext);

int system_authentication_load_user(system_ctx_t *ctx, UT_array **arr)
{
	int error = 0;

	struct passwd *pwd = NULL;
	struct spwd *pwdshd = NULL;

	system_local_user_t tmp_user = {0};

	// adding username
	while ((pwd = getpwent()) != NULL) {
		if ((pwd->pw_uid >= 1000 && strncmp(pwd->pw_dir, "/home", sizeof("/home") - 1) == 0) || (pwd->pw_uid == 0)) {
			tmp_user.name = pwd->pw_name;

			error = system_local_user_array_add(arr, tmp_user);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_array_add() error (%d) for %s", error, tmp_user.name);
				goto error_out;
			}
		}
	}

	// adding password
	while ((pwdshd = getspent()) != NULL) {
		tmp_user.name = pwdshd->sp_namp;

		system_local_user_t *found = utarray_find(*arr, &tmp_user, system_local_user_cmp_fn);
		if (found) {
			/* A password field which starts with a exclamation mark means that
			 * the password is locked.
			 * Majority of root accounts are paswordless and therefore contain
			 * only asterisk in the password field.
			 * Set the value to NULL in both cases.
			 */
			if (strcmp(pwdshd->sp_pwdp, "!") != 0 && strcmp(pwdshd->sp_pwdp, "*") != 0) {
				system_local_user_set_password(found, pwdshd->sp_pwdp);
			}
		}
	}

	goto out;

error_out:
	error = -1;

out:
	endpwent();

	return error;
}

int system_authentication_load_user_authorized_key(system_ctx_t *ctx, const char *user, UT_array **arr)
{
	int error = 0;
	char dir_buffer[PATH_MAX] = {0};
	char path_buffer[PATH_MAX] = {0};
	char algorithm_buffer[100] = {0};
	char data_buffer[16384] = {0};
	FILE *pub_file = NULL;

	system_authorized_key_t tmp_key = {0};

	DIR *dir = NULL;
	struct dirent *dir_entry = NULL;

	if (!strncmp(user, "root", 4)) {
		if (snprintf(dir_buffer, sizeof(dir_buffer), "/root/.ssh") < 0) {
			goto error_out;
		}
	} else {
		if (snprintf(dir_buffer, sizeof(dir_buffer), "/home/%s/.ssh", user) < 0) {
			goto error_out;
		}
	}

	// check dir for .ssh key files
	if ((dir = opendir(dir_buffer)) == NULL) {
		SRPLG_LOG_INF(PLUGIN_NAME, ".ssh directory doesn't exist for user %s", user);
		goto out;
	} else {
		while ((dir_entry = readdir(dir)) != NULL) {
			if (!system_check_file_extension(dir_entry->d_name, ".pub")) {
				continue;
			}

			// found new key
			system_authorized_key_init(&tmp_key);
			error = system_authorized_key_set_name(&tmp_key, dir_entry->d_name);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_set_name() error (%d)", error);
				goto error_out;
			}

			// found .pub file - read data
			error = snprintf(path_buffer, sizeof(path_buffer), "%s/%s", dir_buffer, dir_entry->d_name);
			if (error < 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error (%d)", error);
				goto error_out;
			}

			// open file
			pub_file = fopen(path_buffer, "r");
			if (!pub_file) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "fopen() error (%d)", error);
				goto error_out;
			}

			// read algorithm and data
			error = fscanf(pub_file, "%s %s", algorithm_buffer, data_buffer);
			if (error != 2) {
				// unable to read both parameters needed - error
				SRPLG_LOG_ERR(PLUGIN_NAME, "fscanf() error (%d)", error);
				goto error_out;
			}

			// got both - setup key and add it to the array
			error = system_authorized_key_set_algorithm(&tmp_key, algorithm_buffer);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_set_algorithm() error (%d)", error);
				goto error_out;
			}

			error = system_authorized_key_set_data(&tmp_key, data_buffer);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_set_data() error (%d)", error);
				goto error_out;
			}

			// add to the array
			error = system_authorized_key_array_add(arr, tmp_key);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_array_add() error (%d)", error);
				goto error_out;
			}

			// free current data
			system_authorized_key_free(&tmp_key);

			// close current file
			fclose(pub_file);
			pub_file = NULL;
		}
	}

	goto out;

error_out:
	error = -1;

out:
	// close if interrupted
	if (pub_file) {
		fclose(pub_file);
	}

	// if interrurpted the key will have allocated data - free
	system_authorized_key_free(&tmp_key);

	// close dir iterator
	closedir(dir);
	return error;
}

static int system_check_file_extension(const char *path, const char *ext)
{
	const size_t path_len = strlen(path);
	const size_t ext_len = strlen(ext);

	return path_len > ext_len && strncmp(path + (path_len - ext_len), ext, ext_len) == 0;
}