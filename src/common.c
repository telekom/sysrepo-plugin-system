/*
 * telekom / sysrepo-plugin-system
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2021 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "common.h"
#include "utils/memory.h"

#include <sysrepo.h>

#include <stdbool.h>
#include <errno.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/dir.h>
#include <linux/sysinfo.h>
#include <string.h>

int system_get_contact_info(char contact_info[])
{

	struct passwd *pwd = {0};

	setpwent();

	pwd = getpwent();

	if (pwd == NULL) {
		return -1;
	}

	do {
		if (strcmp(pwd->pw_name, CONTACT_USERNAME) == 0) {
			strncpy(contact_info, pwd->pw_gecos, strnlen(pwd->pw_gecos, MAX_GECOS_LEN));
		}
	} while ((pwd = getpwent()) != NULL);

	endpwent();

	return 0;
}

int system_get_timezone_name(char *value)
{
	char buf[TIMEZONE_NAME_LEN];
	ssize_t len = 0;
	size_t start = 0;

	len = readlink(LOCALTIME_FILE, buf, sizeof(buf) - 1);
	if (len == -1) {
		return -1;
	}

	buf[len] = '\0';

	if (strncmp(buf, TIMEZONE_DIR, strlen(TIMEZONE_DIR)) != 0) {
		return -1;
	}

	start = strlen(TIMEZONE_DIR);
	strncpy(value, &buf[start], strnlen(buf, TIMEZONE_NAME_LEN));

	return 0;
}

int system_get_os_info(char **os_name, char **os_release, char **os_version, char **machine)
{
	struct utsname uname_data = {0};

	if (uname(&uname_data) < 0) {
		return -1;
	}

	*os_name = xstrndup(uname_data.sysname, strnlen(uname_data.sysname, UTS_LEN + 1));
	*os_release = xstrndup(uname_data.release, strnlen(uname_data.release, UTS_LEN + 1));
	*os_version = xstrndup(uname_data.version, strnlen(uname_data.version, UTS_LEN + 1));
	*machine = xstrndup(uname_data.machine, strnlen(uname_data.machine, UTS_LEN + 1));

	return 0;
}

int system_get_datetime_info(char current_datetime[], char boot_datetime[])
{
	time_t now = 0;
	struct tm *ts = {0};
	struct sysinfo s_info = {0};
	time_t uptime_seconds = 0;

	now = time(NULL);

	ts = localtime(&now);
	if (ts == NULL) {
		return -1;
	}

	/* must satisfy constraint:
		"\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(\.\d+)?(Z|[\+\-]\d{2}:\d{2})"
		TODO: Add support for:
			- 2021-02-09T06:02:39.234+01:00
			- 2021-02-09T06:02:39.234Z
			- 2021-02-09T06:02:39+11:11
	*/

	strftime(current_datetime, DATETIME_BUF_SIZE, "%FT%TZ", ts);

	if (sysinfo(&s_info) != 0) {
		return -1;
	}

	uptime_seconds = s_info.uptime;

	time_t diff = now - uptime_seconds;

	ts = localtime(&diff);
	if (ts == NULL) {
		return -1;
	}

	strftime(boot_datetime, DATETIME_BUF_SIZE, "%FT%TZ", ts);

	return 0;
}

char *system_get_plugin_file_path(const char *filename, bool create)
{
	// TODO: update this appropriately
	int error = 0;
	char *plugin_dir = NULL;
	char *file_path = NULL;
	size_t filename_len = 0;
	FILE *tmp = NULL;

	plugin_dir = getenv(PLUGIN_DIR_ENV_VAR);
	if (plugin_dir == NULL) {
		//SRPLG_LOG_WRN(PLUGIN_NAME, "Unable to get env var %s", PLUGIN_DIR_ENV_VAR);
		//SRPLG_LOG_INF(PLUGIN_NAME, "Setting the plugin data dir to: %s", PLUGIN_DIR_DEFAULT);
		plugin_dir = PLUGIN_DIR_DEFAULT;
	}

	// check if plugin_dir exists
	DIR *dir = opendir(plugin_dir);
	if (dir) {
		// dir exists
		closedir(dir);
	} else if (ENOENT == errno) {
		error = mkdir(plugin_dir, 0777);
		if (error == -1) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "Error creating dir: %s", plugin_dir);
			return NULL;
		}
	} else {
		SRPLG_LOG_ERR(PLUGIN_NAME, "opendir failed");
		return NULL;
	}

	filename_len = strlen(plugin_dir) + strlen(filename) + 1;
	file_path = xmalloc(filename_len);

	if (snprintf(file_path, filename_len, "%s%s", plugin_dir, filename) < 0) {
		return NULL;
	}

	// check if file exists
	if (access(file_path, F_OK) != 0) {
		if (create) {
			tmp = fopen(file_path, "w");
			if (tmp == NULL) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "Error creating %s", file_path);
				return NULL;
			}
			fclose(tmp);
		} else {
			SRPLG_LOG_ERR(PLUGIN_NAME, "Filename %s doesn't exist in dir %s", filename, plugin_dir);
			return NULL;
		}
	}

	return file_path;
}

int system_get_location(char *location)
{
	FILE *fp = NULL;
	char *location_file_path = NULL;

	location_file_path = system_get_plugin_file_path(LOCATION_FILENAME, false);
	if (location_file_path == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "get_location: couldn't get location file path");
		return -1;
	}

	fp = fopen(location_file_path, "r");
	if (fp == NULL) {
		FREE_SAFE(location_file_path);
		return -1;
	}

	if (fgets(location, MAX_LOCATION_LENGTH, fp) == NULL) {
		fclose(fp);
		fp = NULL;
		FREE_SAFE(location_file_path);
		return -1;
	}

	fclose(fp);
	fp = NULL;
	FREE_SAFE(location_file_path);
	return 0;
}
