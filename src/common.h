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

#ifndef GENERAL_PLUGIN_COMMON_H
#define GENERAL_PLUGIN_COMMON_H

#include <stdbool.h>

#define PLUGIN_NAME "system-plugin"

#define BASE_YANG_MODEL "ietf-system"
#define SYSTEM_YANG_MODEL "/" BASE_YANG_MODEL ":system"

#define SYSREPOCFG_EMPTY_CHECK_COMMAND "sysrepocfg -X -d running -m " BASE_YANG_MODEL

#define SET_CURR_DATETIME_YANG_PATH "/" BASE_YANG_MODEL ":set-current-datetime"
#define RESTART_YANG_PATH "/" BASE_YANG_MODEL ":system-restart"
#define SHUTDOWN_YANG_PATH "/" BASE_YANG_MODEL ":system-shutdown"

#define CONTACT_YANG_PATH SYSTEM_YANG_MODEL "/contact"
#define HOSTNAME_YANG_PATH SYSTEM_YANG_MODEL "/hostname"
#define LOCATION_YANG_PATH SYSTEM_YANG_MODEL "/location"
#define NTP_YANG_PATH SYSTEM_YANG_MODEL "/ntp"
#define DNS_RESOLVER_YANG_PATH SYSTEM_YANG_MODEL "/dns-resolver"
#define AUTHENTICATION_USER_YANG_PATH SYSTEM_YANG_MODEL "/authentication/user"

#define CLOCK_YANG_PATH SYSTEM_YANG_MODEL "/clock"
#define TIMEZONE_NAME_YANG_PATH CLOCK_YANG_PATH "/timezone-name"
#define TIMEZONE_OFFSET_YANG_PATH CLOCK_YANG_PATH "/timezone-utc-offset"

#define NTP_ENABLED_YANG_PATH NTP_YANG_PATH "/enabled"
#define NTP_SERVER_YANG_PATH NTP_YANG_PATH "/server"

#define DNS_RESOLVER_SEARCH_YANG_PATH DNS_RESOLVER_YANG_PATH "/search"
#define DNS_RESOLVER_SERVER_YANG_PATH DNS_RESOLVER_YANG_PATH "/server"
#define DNS_RESOLVER_OPTIONS_YANG_PATH DNS_RESOLVER_YANG_PATH "/options"
#define DNS_RESOLVER_OPTIONS_TIMEOUT_YANG_PATH DNS_RESOLVER_OPTIONS_YANG_PATH "/timeout"
#define DNS_RESOLVER_OPTIONS_ATTEMPTS_YANG_PATH DNS_RESOLVER_OPTIONS_YANG_PATH "/attempts"

#define SYSTEM_STATE_YANG_MODEL "/" BASE_YANG_MODEL ":system-state"
#define STATE_PLATFORM_YANG_PATH SYSTEM_STATE_YANG_MODEL "/platform"
#define STATE_CLOCK_YANG_PATH SYSTEM_STATE_YANG_MODEL "/clock"

#define OS_NAME_YANG_PATH STATE_PLATFORM_YANG_PATH "/os-name"
#define OS_RELEASE_YANG_PATH STATE_PLATFORM_YANG_PATH "/os-release"
#define OS_VERSION_YANG_PATH STATE_PLATFORM_YANG_PATH "/os-version"
#define OS_MACHINE_YANG_PATH STATE_PLATFORM_YANG_PATH "/machine"

#define CURR_DATETIME_YANG_PATH STATE_CLOCK_YANG_PATH "/current-datetime"
#define BOOT_DATETIME_YANG_PATH STATE_CLOCK_YANG_PATH "/boot-datetime"

#define CONTACT_USERNAME "root"
#define CONTACT_TEMP_FILE "/tmp/tempfile"
#define PASSWD_BAK_FILE PASSWD_FILE ".bak"
#define MAX_GECOS_LEN 100

#define TIMEZONE_DIR "/usr/share/zoneinfo/"
#define LOCALTIME_FILE "/etc/localtime"
#define ZONE_DIR_LEN 20			 // '/usr/share/zoneinfo' length
#define TIMEZONE_NAME_LEN 14 * 3 // The Area and Location names have a maximum length of 14 characters, but areas can have a subarea

#define DATETIME_BUF_SIZE 30
#define UTS_LEN 64

#define PLUGIN_DIR_DEFAULT "/usr/local/lib/sysrepo-general-plugin"
#define LOCATION_FILENAME "/location_info"
#define PLUGIN_DIR_ENV_VAR "GEN_PLUGIN_DATA_DIR"
#define MAX_LOCATION_LENGTH 100

#define NTP_NAMES_FILENAME "/ntp_names"
#define NTP_TMP_NAMES_FILENAME "/tmp_ntp_names"
#define NTP_MAX_ENTRY_LEN 100
#define PATH_MAX_BUFFER 200

char *system_get_plugin_file_path(const char *filename, bool create);
int system_get_contact_info(char contact_info[]);
int system_get_timezone_name(char *value);
int system_get_os_info(char **os_name, char **os_release, char **os_version, char **machine);
int system_get_datetime_info(char current_datetime[], char boot_datetime[]);
int system_get_location(char *location);

#endif // GENERAL_PLUGIN_COMMON_H