/*
 * telekom / sysrepo-plugin-system
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2022 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "load.h"
#include "sysrepo.h"
#include "plugin/types.h"
#include "plugin/common.h"

#include "plugin/data/system/authentication/authorized_key/list.h"
#include "plugin/data/system/authentication/authorized_key.h"
#include "plugin/data/system/authentication/local_user/list.h"
#include "plugin/data/system/authentication/local_user.h"
#include "umgmt/user.h"

#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <linux/limits.h>

#include <umgmt.h>

#include <utlist.h>

static int system_check_file_extension(const char *path, const char *ext);

int system_authentication_load_user(system_ctx_t *ctx, system_local_user_element_t **head)
{
	int error = 0;

	system_local_user_t temp_user = {0};
	um_db_t *db = NULL;
	const um_user_element_t *user_head = NULL;
	const um_user_element_t *user_iter = NULL;

	db = um_db_new();
	if (!db) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "um_db_new() failed");
		goto error_out;
	}

	error = um_db_load(db);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "um_db_load() error (%d)", error);
		goto error_out;
	}

	user_head = um_db_get_user_list_head(db);

	LL_FOREACH(user_head, user_iter)
	{
		const um_user_t *user = user_iter->user;

		if (um_user_get_uid(user) == 0 || (um_user_get_uid(user) >= 1000 && um_user_get_uid(user) < 65534)) {
			SRPLG_LOG_INF(PLUGIN_NAME, "Found user %s [ UID = %d ]", um_user_get_name(user), um_user_get_uid(user));

			// add new user
			system_local_user_init(&temp_user);

			temp_user.name = (char *) um_user_get_name(user);
			if (um_user_get_password_hash(user) &&
				strcmp(um_user_get_password_hash(user), "*") &&
				strcmp(um_user_get_password_hash(user), "!")) {
				temp_user.password = (char *) um_user_get_password_hash(user);
			}

			error = system_local_user_list_add(head, temp_user);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_add() error (%d) for user %s", error, temp_user.name);
				goto error_out;
			}
		}
	}

	goto out;

error_out:
	error = -1;

out:
	if (db) {
		um_db_free(db);
	}

	return error;
}

int system_authentication_load_user_authorized_key(system_ctx_t *ctx, const char *user, system_authorized_key_element_t **head)
{
	int error = 0;
	char dir_buffer[PATH_MAX] = {0};
	char path_buffer[PATH_MAX] = {0};
	char algorithm_buffer[100] = {0};
	char data_buffer[16384] = {0};
	FILE *pub_file = NULL;

	system_authorized_key_t temp_key = {0};

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
		SRPLG_LOG_INF(PLUGIN_NAME, "~/.ssh directory doesn't exist for user %s", user);
		goto out;
	} else {
		while ((dir_entry = readdir(dir)) != NULL) {
			if (!system_check_file_extension(dir_entry->d_name, ".pub")) {
				continue;
			}

			// found new key
			system_authorized_key_init(&temp_key);
			error = system_authorized_key_set_name(&temp_key, dir_entry->d_name);
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
			error = system_authorized_key_set_algorithm(&temp_key, algorithm_buffer);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_set_algorithm() error (%d)", error);
				goto error_out;
			}

			error = system_authorized_key_set_data(&temp_key, data_buffer);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_set_data() error (%d)", error);
				goto error_out;
			}

			// append to list
			error = system_authorized_key_list_add(head, temp_key);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_list_add() error (%d)", error);
				goto error_out;
			}

			// free current data
			system_authorized_key_free(&temp_key);

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
	system_authorized_key_free(&temp_key);

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