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

#include <unistd.h>
#include <linux/limits.h>

#include <sysrepo.h>

int system_load_hostname(system_ctx_t *ctx, char buffer[SYSTEM_HOSTNAME_LENGTH_MAX])
{
	int error = 0;

	error = gethostname(buffer, SYSTEM_HOSTNAME_LENGTH_MAX);
	if (error) {
		return -1;
	}

	return 0;
}

int system_load_contact(system_ctx_t *ctx, char buffer[256])
{
	int error = 0;
	return error;
}

int system_load_location(system_ctx_t *ctx, char buffer[256])
{
	int error = 0;
	return error;
}

int system_load_timezone_name(system_ctx_t *ctx, char buffer[SYSTEM_TIMEZONE_NAME_LENGTH_MAX])
{
	int error = 0;

	char timezone_path_buffer[PATH_MAX] = {0};

	ssize_t len = 0;
	size_t start = 0;

	len = readlink(SYSTEM_LOCALTIME_FILE, timezone_path_buffer, sizeof(timezone_path_buffer) - 1);
	if (len == -1) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "readlink() error");
		goto error_out;
	}

	// terminate path
	timezone_path_buffer[len] = 0;

	// assert start is equal to the timezone dir path
	if (strncmp(timezone_path_buffer, SYSTEM_TIMEZONE_DIR, sizeof(SYSTEM_TIMEZONE_DIR) - 1) != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "strncmp() error");
		goto error_out;
	}

	// fetch the rest of the path into timezone_name_buffer
	start = sizeof(SYSTEM_TIMEZONE_DIR);
	strcpy(buffer, &timezone_path_buffer[start]);

	goto out;

error_out:
	error = -1;

out:
	return error;
}