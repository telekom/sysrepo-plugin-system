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
#include "operational.h"
#include "common.h"
#include "ly_tree.h"

#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <string.h>
#include <errno.h>

#include <sysrepo.h>
#include <assert.h>

struct system_platform {
	char *os_name;
	char *os_release;
	char *os_version;
	char *machine;
};

struct system_clock {
	char current_datetime[SYSTEM_DATETIME_BUFFER_SIZE];
	char boot_datetime[SYSTEM_DATETIME_BUFFER_SIZE];
};

// helpers //

static int system_get_platform_info(struct system_platform *platform);
static void system_free_platform_info(struct system_platform *platform);
static int system_get_clock_info(struct system_clock *clock);

////

int system_subscription_operational_platform(sr_session_ctx_t *session, uint32_t sub_id, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data)
{
	int error = SR_ERR_OK;
	struct system_platform platform = {0};
	const struct ly_ctx *ly_ctx = NULL;
	struct lyd_node *platform_container_node = *parent;

	error = system_get_platform_info(&platform);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_get_platform_info() error: %s", strerror(errno));
		goto error_out;
	}

	if (*parent == NULL) {
		ly_ctx = sr_acquire_context(sr_session_get_connection(session));
		if (ly_ctx == NULL) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
			goto error_out;
		}
	}

	// make sure the passed parent node is the platform container node - the one we subscribed to
	assert(strcmp(LYD_NAME(platform_container_node), "platform") == 0);

	error = system_ly_tree_create_state_platform_os_name(ly_ctx, platform_container_node, platform.os_name);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_state_platform_os_name() error (%d)", error);
		goto error_out;
	}

	error = system_ly_tree_create_state_platform_os_release(ly_ctx, platform_container_node, platform.os_release);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_state_platform_os_release() error (%d)", error);
		goto error_out;
	}

	error = system_ly_tree_create_state_platform_os_version(ly_ctx, platform_container_node, platform.os_version);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_state_platform_os_version() error (%d)", error);
		goto error_out;
	}

	error = system_ly_tree_create_state_platform_machine(ly_ctx, platform_container_node, platform.machine);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_state_platform_machine() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;
out:

	system_free_platform_info(&platform);

	return error;
}

int system_subscription_operational_clock(sr_session_ctx_t *session, uint32_t sub_id, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data)
{
	int error = SR_ERR_OK;
	struct system_clock clock = {0};
	const struct ly_ctx *ly_ctx = NULL;
	struct lyd_node *clock_container_node = *parent;

	error = system_get_clock_info(&clock);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_get_platform_info() error: %s", strerror(errno));
		goto error_out;
	}

	if (*parent == NULL) {
		ly_ctx = sr_acquire_context(sr_session_get_connection(session));
		if (ly_ctx == NULL) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_acquire_context() failed");
			goto error_out;
		}
	}

	// make sure the passed parent node is the clock container node - the one we subscribed to
	assert(strcmp(LYD_NAME(clock_container_node), "clock") == 0);

	error = system_ly_tree_create_state_clock_current_datetime(ly_ctx, clock_container_node, clock.current_datetime);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_state_clock_current_datetime() error (%d)", error);
		goto error_out;
	}

	error = system_ly_tree_create_state_clock_boot_datetime(ly_ctx, clock_container_node, clock.boot_datetime);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_ly_tree_create_state_clock_boot_datetime() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;
out:

	return error;
}

static int system_get_platform_info(struct system_platform *platform)
{
	struct utsname uname_data = {0};

	if (uname(&uname_data) < 0) {
		return -1;
	}

	platform->os_name = strndup(uname_data.sysname, strnlen(uname_data.sysname, SYSTEM_UTS_LEN + 1));
	platform->os_release = strndup(uname_data.release, strnlen(uname_data.release, SYSTEM_UTS_LEN + 1));
	platform->os_version = strndup(uname_data.version, strnlen(uname_data.version, SYSTEM_UTS_LEN + 1));
	platform->machine = strndup(uname_data.machine, strnlen(uname_data.machine, SYSTEM_UTS_LEN + 1));

	return 0;
}

static void system_free_platform_info(struct system_platform *platform)
{
	if (platform->os_name) {
		free(platform->os_name);
	}
	if (platform->os_release) {
		free(platform->os_release);
	}
	if (platform->os_version) {
		free(platform->os_version);
	}
	if (platform->machine) {
		free(platform->machine);
	}
}

static int system_get_clock_info(struct system_clock *clock)
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

	strftime(clock->current_datetime, SYSTEM_DATETIME_BUFFER_SIZE, "%FT%TZ", ts);

	if (sysinfo(&s_info) != 0) {
		return -1;
	}

	uptime_seconds = s_info.uptime;

	time_t diff = now - uptime_seconds;

	ts = localtime(&diff);
	if (ts == NULL) {
		return -1;
	}

	strftime(clock->boot_datetime, SYSTEM_DATETIME_BUFFER_SIZE, "%FT%TZ", ts);

	return 0;
}