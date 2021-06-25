#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/sendfile.h>
#include <sys/utsname.h>
#include <sys/reboot.h>
#include <fcntl.h>
#define __USE_XOPEN // needed for strptime
#include <time.h>
#include <sysrepo/xpath.h>
#include "general.h"
#include "utils/memory.h"
#include "utils/ntp/server_list.h"
#include "utils/dns/search.h"
#include "utils/dns/server.h"
#ifdef SYSTEMD
#else
#include "utils/dns/resolv_conf.h"
#endif
#include "utils/user_auth/user_authentication.h"

/*
typedef struct {
	char *value;
	char *xpath;
} result_value_t;

typedef struct {
	result_value_t *values;
	size_t num_values;
} result_values_t;
*/
static ntp_server_list_t *ntp_servers;
static dns_server_list_t dns_servers;
static local_user_list_t *user_list;

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

#define LOCATION_FILENAME "/location_info"
#define PLUGIN_DIR_ENV_VAR "GEN_PLUGIN_DATA_DIR"
#define MAX_LOCATION_LENGTH 100

#define NTP_NAMES_FILENAME "/ntp_names"
#define NTP_TMP_NAMES_FILENAME "/tmp_ntp_names"
#define NTP_MAX_ENTRY_LEN 100
#define PATH_MAX_BUFFER 200

static int system_module_change_cb(sr_session_ctx_t *session, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);
static int system_state_data_cb(sr_session_ctx_t *session, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);
static int system_rpc_cb(sr_session_ctx_t *session, const char *op_path, const sr_val_t *input, const size_t input_cnt, sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data);

static bool system_running_datastore_is_empty_check(void);
static int load_data(sr_session_ctx_t *session);
static char *system_xpath_get(const struct lyd_node *node);

static int set_config_value(const char *xpath, const char *value, sr_change_oper_t operation);
static int set_ntp(const char *xpath, char *value);
static int set_dns(const char *xpath, char *value, sr_change_oper_t operation);
#ifndef SYSTEMD
static int set_dns_timeout(char *value);
static int set_dns_attempts(char *value);
#endif
static int set_contact_info(const char *value);
static int set_timezone(const char *value);

static int get_contact_info(char *value);
static int get_timezone_name(char *value);

static int get_os_info(char **os_name, char **os_release, char **os_version, char **machine);
static int get_datetime_info(char current_datetime[], char boot_datetime[]);

static int set_datetime(char *datetime);

char *get_plugin_file_path(const char *filename, bool create);

static int set_location(const char *location);
static int get_location(char *location);

int ntp_set_server_name(char *name, char *address);
int ntp_get_server_name(char **name, char *address);
int ntp_set_entry_datastore(sr_session_ctx_t *session, ntp_server_t *server_entry);

static int set_user_authentication(const char *xpath, char *value);

int sr_plugin_init_cb(sr_session_ctx_t *session, void **private_data)
{
	int error = 0;
	sr_conn_ctx_t *connection = NULL;
	sr_session_ctx_t *startup_session = NULL;
	sr_subscription_ctx_t *subscription = NULL;
	char *location_file_path = NULL;
	char *ntp_names_file_path = NULL;
	*private_data = NULL;

	location_file_path = get_plugin_file_path(LOCATION_FILENAME, true);
	if (location_file_path == NULL) {
		SRP_LOG_ERR("Please set the %s env variable. "
					"The plugin uses the path in the variable "
					"to store location in a file.",
					PLUGIN_DIR_ENV_VAR);
		error = -1;
		goto error_out;
	}

	ntp_names_file_path = get_plugin_file_path(NTP_NAMES_FILENAME, true);
	if (ntp_names_file_path == NULL) {
		SRP_LOG_ERR("Please set the %s env variable. "
			       "The plugin uses the path in the variable "
			       "to store ntp server names in a file.", PLUGIN_DIR_ENV_VAR);
		error = -1;
		goto error_out;
	}

	dns_server_list_init(&dns_servers);

	local_user_list_init(&user_list);

	SRP_LOG_INFMSG("start session to startup datastore");

	connection = sr_session_get_connection(session);
	error = sr_session_start(connection, SR_DS_STARTUP, &startup_session);
	if (error) {
		SRP_LOG_ERR("sr_session_start error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	*private_data = startup_session;

	error = ntp_server_list_init(session, &ntp_servers);
	if (error != 0) {
		SRP_LOG_ERR("ntp_server_list_init error: %s", strerror(errno));
		goto error_out;
	}

	if (system_running_datastore_is_empty_check() == true) {
		SRP_LOG_INFMSG("running DS is empty, loading data");

		error = load_data(session);
		if (error) {
			SRP_LOG_ERRMSG("load_data error");
			goto error_out;
		}

		error = sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0, 0);
		if (error) {
			SRP_LOG_ERR("sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	SRP_LOG_INFMSG("subscribing to module change");

	error = sr_module_change_subscribe(session, BASE_YANG_MODEL, "/" BASE_YANG_MODEL ":*//*", system_module_change_cb, *private_data, 0, SR_SUBSCR_DEFAULT, &subscription);
	if (error) {
		SRP_LOG_ERR("sr_module_change_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	SRP_LOG_INFMSG("subscribing to get oper items");

	error = sr_oper_get_items_subscribe(session, BASE_YANG_MODEL, SYSTEM_STATE_YANG_MODEL, system_state_data_cb, NULL, SR_SUBSCR_CTX_REUSE, &subscription);
	if (error) {
		SRP_LOG_ERR("sr_oper_get_items_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	SRP_LOG_INFMSG("subscribing to rpc");

	error = sr_rpc_subscribe(session, SET_CURR_DATETIME_YANG_PATH, system_rpc_cb, *private_data, 0, SR_SUBSCR_CTX_REUSE, &subscription);
	if (error) {
		SRP_LOG_ERR("sr_rpc_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	error = sr_rpc_subscribe(session, RESTART_YANG_PATH, system_rpc_cb, *private_data, 0, SR_SUBSCR_CTX_REUSE, &subscription);
	if (error) {
		SRP_LOG_ERR("sr_rpc_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	error = sr_rpc_subscribe(session, SHUTDOWN_YANG_PATH, system_rpc_cb, *private_data, 0, SR_SUBSCR_CTX_REUSE, &subscription);
	if (error) {
		SRP_LOG_ERR("sr_rpc_subscribe error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	SRP_LOG_INFMSG("plugin init done");

	FREE_SAFE(location_file_path);
	FREE_SAFE(ntp_names_file_path);

	goto out;

error_out:
	if (subscription != NULL) {
		sr_unsubscribe(subscription);
	}

	if (location_file_path != NULL) {
		FREE_SAFE(location_file_path);
	}

	if (ntp_names_file_path != NULL) {
		FREE_SAFE(ntp_names_file_path);
	}

out:
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static bool system_running_datastore_is_empty_check(void)
{
	FILE *sysrepocfg_DS_empty_check = NULL;
	bool is_empty = false;

	sysrepocfg_DS_empty_check = popen(SYSREPOCFG_EMPTY_CHECK_COMMAND, "r");
	if (sysrepocfg_DS_empty_check == NULL) {
		SRP_LOG_WRN("could not execute %s", SYSREPOCFG_EMPTY_CHECK_COMMAND);
		is_empty = true;
		goto out;
	}

	if (fgetc(sysrepocfg_DS_empty_check) == EOF) {
		is_empty = true;
	}

out:
	if (sysrepocfg_DS_empty_check) {
		pclose(sysrepocfg_DS_empty_check);
	}

	return is_empty;
}

char *get_plugin_file_path(const char *filename, bool create)
{
	char *plugin_dir = NULL;
	char *file_path = NULL;
	size_t filename_len = 0;
	FILE *tmp = NULL;

	plugin_dir = getenv(PLUGIN_DIR_ENV_VAR);
	if (plugin_dir == NULL) {
		SRP_LOG_ERR("Unable to get env var %s", PLUGIN_DIR_ENV_VAR);
		return NULL;
	}

	filename_len = strlen(plugin_dir) + strlen(filename) + 1;
	file_path = xmalloc(filename_len);

	if (snprintf(file_path, filename_len, "%s%s", plugin_dir, filename) < 0) {
		FREE_SAFE(file_path);
		return NULL;
	}

	// check if file exists
	if (access(file_path, F_OK) != 0) {
		if (create) {
			tmp = fopen(file_path, "w");
			if (tmp == NULL) {
				SRP_LOG_ERR("Error creating %s", file_path);
				FREE_SAFE(file_path);
				return NULL;
			}
			fclose(tmp);
		} else {
			SRP_LOG_ERR("Filename %s doesn't exist in dir %s", filename, plugin_dir);
			FREE_SAFE(file_path);
			return NULL;
		}
	}

	return file_path;
}

static int load_data(sr_session_ctx_t *session)
{
	int error = 0;
	char contact_info[MAX_GECOS_LEN] = {0};
	char hostname[HOST_NAME_MAX] = {0};
	char location[MAX_LOCATION_LENGTH] = {0};
	char tmp_buffer[PATH_MAX_BUFFER];
	char *location_file_path = NULL;
	struct stat stat_buf = {0};

	// get the contact info from /etc/passwd
	error = get_contact_info(contact_info);
	if (error) {
		SRP_LOG_ERR("get_contact_info error: %s", strerror(errno));
		goto error_out;
	}

	error = sr_set_item_str(session, CONTACT_YANG_PATH, contact_info, NULL, SR_EDIT_DEFAULT);
	if (error) {
		SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	// get the hostname of the system
	error = gethostname(hostname, HOST_NAME_MAX);
	if (error != 0) {
		SRP_LOG_ERR("gethostname error: %s", strerror(errno));
		goto error_out;
	}

	error = sr_set_item_str(session, HOSTNAME_YANG_PATH, hostname, NULL, SR_EDIT_DEFAULT);
	if (error) {
		SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	// TODO: comment out for now because: "if-feature timezone-name;"
	//		 the feature has to be enabled in order to set the item
	/*
	char timezone_name[TIMEZONE_NAME_LEN] = {0};
	// get the current datetime (timezone-name) of the system
	error = get_timezone_name(timezone_name);
	if (error != 0) {
		SRP_LOG_ERR("get_timezone_name error: %s", strerror(errno));
		goto error_out;
	}

	error = sr_set_item_str(session, TIMEZONE_NAME_YANG_PATH, timezone_name, NULL, SR_EDIT_DEFAULT);
	if (error) {
		SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}
	*/

	// TODO: move this to a separate function
	for (int i = 0; i < user_list->count; i++) {
		error = snprintf(tmp_buffer, sizeof(tmp_buffer), "/ietf-system:system/authentication/user[name='%s']/name", user_list->users[i].name);
		if (error < 0) {
			// snprintf error
			SRP_LOG_ERRMSG("snprintf failed");
			goto error_out;
		}
		error = sr_set_item_str(session, tmp_buffer,  user_list->users[i].name, NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		if (!user_list->users[i].nologin) {
			error = snprintf(tmp_buffer, sizeof(tmp_buffer), "/ietf-system:system/authentication/user[name='%s']/password", user_list->users[i].name);
			if (error < 0) {
				// snprintf error
				SRP_LOG_ERRMSG("snprintf failed");
				goto error_out;
			}
			error = sr_set_item_str(session, tmp_buffer,  user_list->users[i].password, NULL, SR_EDIT_DEFAULT);
			if (error) {
				SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
				goto error_out;
			}
		}

		for (int j = 0; j < user_list->users[i].auth.count; j++) {
			error = snprintf(tmp_buffer, sizeof(tmp_buffer), "/ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']/name", user_list->users[i].name, user_list->users[i].auth.authorized_keys[j].name);
			if (error < 0) {
				// snprintf error
				SRP_LOG_ERRMSG("snprintf failed");
				goto error_out;
			}
			error = sr_set_item_str(session, tmp_buffer,  user_list->users[i].auth.authorized_keys[j].name, NULL, SR_EDIT_DEFAULT);
			if (error) {
				SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
				goto error_out;
			}

			error = snprintf(tmp_buffer, sizeof(tmp_buffer), "/ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']/algorithm", user_list->users[i].name, user_list->users[i].auth.authorized_keys[j].name);
			if (error < 0) {
				// snprintf error
				SRP_LOG_ERRMSG("snprintf failed");
				goto error_out;
			}
			error = sr_set_item_str(session, tmp_buffer,  user_list->users[i].auth.authorized_keys[j].algorithm, NULL, SR_EDIT_DEFAULT);
			if (error) {
				SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
				goto error_out;
			}

			error = snprintf(tmp_buffer, sizeof(tmp_buffer), "/ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']/key-data", user_list->users[i].name, user_list->users[i].auth.authorized_keys[j].name);
			if (error < 0) {
				// snprintf error
				SRP_LOG_ERRMSG("snprintf failed");
				goto error_out;
			}
			error = sr_set_item_str(session, tmp_buffer,  user_list->users[i].auth.authorized_keys[j].key_data, NULL, SR_EDIT_DEFAULT);
			if (error) {
				SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
				goto error_out;
			}
		}
	}

	// check if the location file is not empty
	location_file_path = get_plugin_file_path(LOCATION_FILENAME, false);
	if (location_file_path == NULL) {
		SRP_LOG_ERRMSG("get_plugin_file_path: couldn't get location file path");
		goto error_out;
	}

	error = stat(location_file_path, &stat_buf);
	if (error == -1) {
		SRP_LOG_ERR("stat error (%d): %s", error, strerror(errno));
		goto error_out;
	}

	// if it's not empty, get the location
	if(stat_buf.st_size >= 1) {
		// get the location of the system
		error = get_location(location);
		if (error != 0) {
			SRP_LOG_ERR("get_location error: %s", strerror(errno));
			goto error_out;
		}

		error = sr_set_item_str(session, LOCATION_YANG_PATH, location, NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	// TODO: add ntp server info to datastore
	//set_ntp_servers_sr_items(ntp_servers);

	error = sr_apply_changes(session, 0, 0);
	if (error) {
		SRP_LOG_ERR("sr_apply_changes error (%d): %s", error, sr_strerror(error));
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

int ntp_set_entry_datastore(sr_session_ctx_t *session, ntp_server_t *server_entry)
{
	int error = 0;
	char ntp_path_buffer[PATH_MAX] = {0};
	char xpath_buffer[PATH_MAX] = {0};

	// setup the xpath
	// example xpath: 	"ietf-system:system/ntp/server[name='hr3.pool.ntp.org']/udp/address"
	error = snprintf(ntp_path_buffer, sizeof(ntp_path_buffer) / sizeof(char), "%s[name=\"%s\"]", NTP_SERVER_YANG_PATH, server_entry->name);
	if (error < 0) {
		SRP_LOG_ERRMSG("snprintf error");
		goto error_out;
	}

	// name
	error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/name", ntp_path_buffer);
	if (error < 0) {
		SRP_LOG_ERRMSG("snprintf error");
		goto error_out;
	}

	error = sr_set_item_str(session, xpath_buffer, server_entry->name, NULL, SR_EDIT_DEFAULT);
	if (error) {
		SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	// address
	error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/udp/address", ntp_path_buffer);
	if (error < 0) {
		SRP_LOG_ERRMSG("snprintf error");
		goto error_out;
	}

	error = sr_set_item_str(session, xpath_buffer, server_entry->address, NULL, SR_EDIT_DEFAULT);
	if (error) {
		SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	// association type
	error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/association-type", ntp_path_buffer);
	if (error < 0) {
		SRP_LOG_ERRMSG("snprintf error");
		goto error_out;
	}

	error = sr_set_item_str(session, xpath_buffer, server_entry->assoc_type, NULL, SR_EDIT_DEFAULT);
	if (error) {
		SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	// port, iburst and prefer can be NULL
	// port
	if (server_entry->port != NULL) {
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/udp/port", ntp_path_buffer);
		if (error < 0) {
			SRP_LOG_ERRMSG("snprintf error");
			goto error_out;
		}

		error = sr_set_item_str(session, xpath_buffer, server_entry->port, NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	// iburst
	if (server_entry->iburst != NULL) {
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/iburst", ntp_path_buffer);
		if (error < 0) {
			SRP_LOG_ERRMSG("snprintf error");
			goto error_out;
		}

		error = sr_set_item_str(session, xpath_buffer, (strlen(server_entry->iburst) > 0) ? "true" : "false", NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	// prefer
	if (server_entry->prefer != NULL) {
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/prefer", ntp_path_buffer);
		if (error < 0) {
			SRP_LOG_ERRMSG("snprintf error");
			goto error_out;
		}

		error = sr_set_item_str(session, xpath_buffer, (strlen(server_entry->prefer) > 0) ? "true" : "false", NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	error = sr_apply_changes(session, 0, 0);
	if (error != 0) {
		SRP_LOG_ERR("sr_apply_changes error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	return 0;

error_out:
	return -1;
}

void sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *private_data)
{
	sr_session_ctx_t *startup_session = (sr_session_ctx_t *) private_data;

	if (startup_session) {
		sr_session_stop(startup_session);
	}

	if (ntp_servers) {
		ntp_server_list_free(ntp_servers);
	}

	dns_server_list_free(&dns_servers);
	local_user_list_free(user_list);

	SRP_LOG_INFMSG("plugin cleanup finished");
}

static int system_module_change_cb(sr_session_ctx_t *session, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = 0;
	sr_session_ctx_t *startup_session = (sr_session_ctx_t *) private_data;
	sr_change_iter_t *system_change_iter = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	const struct lyd_node *node = NULL;
	const char *prev_value = NULL;
	const char *prev_list = NULL;
	bool prev_default = false;
	char *node_xpath = NULL;
	const char *node_value = NULL;
	struct lyd_node_leaf_list *node_leaf_list;
	struct lys_node_leaf *schema_node_leaf;
	bool ntp_change = false;
	bool dns_servers_change = false;
	bool user_change = false;

	SRP_LOG_INF("module_name: %s, xpath: %s, event: %d, request_id: %" PRIu32, module_name, xpath, event, request_id);

	if (event == SR_EV_ABORT) {
		SRP_LOG_ERR("aborting changes for: %s", xpath);
		error = -1;
		goto error_out;
	}

	if (event == SR_EV_DONE) {
		error = sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0, 0);
		if (error) {
			SRP_LOG_ERR("sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	if (event == SR_EV_CHANGE) {
		error = sr_get_changes_iter(session, xpath, &system_change_iter);
		if (error) {
			SRP_LOG_ERR("sr_get_changes_iter error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		while (sr_get_change_tree_next(session, system_change_iter, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
			node_xpath = system_xpath_get(node);

			if (node->schema->nodetype == LYS_LEAF || node->schema->nodetype == LYS_LEAFLIST) {
				node_leaf_list = (struct lyd_node_leaf_list *) node;
				node_value = node_leaf_list->value_str;
				if (node_value == NULL) {
					schema_node_leaf = (struct lys_node_leaf *) node_leaf_list->schema;
					node_value = schema_node_leaf->dflt ? schema_node_leaf->dflt : "";
				}
			}

			SRP_LOG_DBG("node_xpath: %s; prev_val: %s; node_val: %s; operation: %d", node_xpath, prev_value, node_value, operation);

			if (node->schema->nodetype == LYS_LEAF || node->schema->nodetype == LYS_LEAFLIST) {
				if (operation == SR_OP_CREATED || operation == SR_OP_MODIFIED) {
					error = set_config_value(node_xpath, node_value, operation);
					if (error) {
						SRP_LOG_ERR("set_config_value error (%d)", error);
						goto error_out;
					}

					if (strncmp(node_xpath, NTP_YANG_PATH, sizeof(NTP_YANG_PATH) - 1) == 0) {
						ntp_change = true;
					}

					if (strncmp(node_xpath, DNS_RESOLVER_SERVER_YANG_PATH, sizeof(DNS_RESOLVER_SERVER_YANG_PATH) - 1) == 0) {
						dns_servers_change = true;
					}

					if (strncmp(node_xpath, AUTHENTICATION_USER_YANG_PATH, strlen(AUTHENTICATION_USER_YANG_PATH)) == 0) {
						user_change = true;
					}
				} else if (operation == SR_OP_DELETED) {
					error = set_config_value(node_xpath, node_value, operation);
					if (error) {
						SRP_LOG_ERR("set_config_value error (%d)", error);
						goto error_out;
					}

					if (strncmp(node_xpath, NTP_YANG_PATH, strlen(NTP_YANG_PATH)) == 0) {
						ntp_change = true;
					}

					if (strncmp(node_xpath, DNS_RESOLVER_SERVER_YANG_PATH, strlen(DNS_RESOLVER_SERVER_YANG_PATH)) == 0) {
						dns_servers_change = true;
					}

					if (strncmp(node_xpath, AUTHENTICATION_USER_YANG_PATH, strlen(AUTHENTICATION_USER_YANG_PATH)) == 0) {
						user_change = true;
					}
				}
			}
			FREE_SAFE(node_xpath);
			node_value = NULL;
		}

		if (ntp_change) {
			// save data to ntp.conf
			error = save_ntp_config(ntp_servers);
			if (error) {
				SRP_LOG_ERR("save_ntp_config error (%d)", error);
				goto error_out;
			}
		}

		if (dns_servers_change == true) {
			SRP_LOG_DBGMSG("Dumping DNS servers configuration...");
			error = dns_server_list_dump_config(&dns_servers);
			if (error != 0) {
				SRP_LOG_ERR("dns_server_list_dump_config (%d)", error);
				goto error_out;
			}
		}

		if (user_change) {
			// save users to system
			error = set_new_users(user_list);
			if (error) {
				SRP_LOG_ERR("set_new_users error (%d)", error);
				goto error_out;
			}
		}
	}
	goto out;

error_out:
	//TODO: handle errors here

out:
	FREE_SAFE(node_xpath);
	sr_free_change_iter(system_change_iter);

	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

static int set_config_value(const char *xpath, const char *value, sr_change_oper_t operation)
{
	int error = 0;
	SRP_LOG_DBGMSG("Setting config value");

	if (strcmp(xpath, HOSTNAME_YANG_PATH) == 0) {
		if (operation == SR_OP_DELETED) {
			error = sethostname("none", strnlen("none", HOST_NAME_MAX));
			if (error != 0) {
				SRP_LOG_ERR("sethostname error: %s", strerror(errno));
			}
		} else {
			error = sethostname(value, strnlen(value, HOST_NAME_MAX));
			if (error != 0) {
				SRP_LOG_ERR("sethostname error: %s", strerror(errno));
			}
		}
	} else if (strcmp(xpath, CONTACT_YANG_PATH) == 0) {
		if (operation == SR_OP_DELETED) {
			error = set_contact_info("");
			if (error != 0) {
				SRP_LOG_ERR("set_contact_info error: %s", strerror(errno));
			}
		} else {
			error = set_contact_info(value);
			if (error != 0) {
				SRP_LOG_ERR("set_contact_info error: %s", strerror(errno));
			}
		}
	} else if (strcmp(xpath, LOCATION_YANG_PATH) == 0) {
		if (operation == SR_OP_DELETED) {
			error = set_location("none");
			if (error != 0) {
				SRP_LOG_ERR("setlocation error: %s", strerror(errno));
			}
		} else {
			error = set_location(value);
			if (error != 0) {
				SRP_LOG_ERR("setlocation error: %s", strerror(errno));
			}
		}
	} else if (strcmp(xpath, TIMEZONE_NAME_YANG_PATH) == 0) {
		if (operation == SR_OP_DELETED) {
			// check if the /etc/localtime symlink exists
			error = access(LOCALTIME_FILE, F_OK);
			if (error != 0) {
				SRP_LOG_ERR("/etc/localtime doesn't exist; unlink/delete timezone error: %s", strerror(errno));
			}

			error = unlink(LOCALTIME_FILE);
			if (error != 0) {
				SRP_LOG_ERR("unlinking/deleting timezone error: %s", strerror(errno));
			}
		} else {
			error = set_timezone(value);
			if (error != 0) {
				SRP_LOG_ERR("set_timezone error: %s", strerror(errno));
			}
		}
	} else if (strcmp(xpath, TIMEZONE_OFFSET_YANG_PATH) == 0) {
		// timezone-utc-offset leaf
		// https://linux.die.net/man/5/tzfile
		// https://linux.die.net/man/8/zic
	} else if (strncmp(xpath, NTP_YANG_PATH, strlen(NTP_YANG_PATH)) == 0) {
		if (operation == SR_OP_DELETED) {
			error = set_ntp(xpath, "");
			if (error != 0) {
				SRP_LOG_ERR("set_ntp error: %s", strerror(errno));
			}
		} else {
			error = set_ntp(xpath, (char *)value);
			if (error != 0) {
				SRP_LOG_ERR("set_ntp error: %s", strerror(errno));
			}
		}
	} else if (strncmp(xpath, DNS_RESOLVER_YANG_PATH, sizeof(DNS_RESOLVER_YANG_PATH) - 1) == 0) {
		error = set_dns(xpath, (char *) value, operation);
		if (error != 0) {
			SRP_LOG_ERRMSG("set_dns error");
		}
	} else if (strncmp(xpath, AUTHENTICATION_USER_YANG_PATH, strlen(AUTHENTICATION_USER_YANG_PATH)) == 0) {
		if (operation == SR_OP_DELETED) {
			error = set_user_authentication(xpath, "");
			if (error != 0) {
				SRP_LOG_ERRMSG("set_authentication_user error");
			}
		} else {
			error = set_user_authentication(xpath, (char *)value);
			if (error != 0) {
				SRP_LOG_ERRMSG("set_authentication_user error");
			}
		}
	}

	return error;
}

static int set_user_authentication(const char *xpath, char *value)
{
	int error = 0;
	char *user_node = NULL;
	char *user_name = NULL;
	char *user_ssh_file_name = NULL;
	char *tmp_ssh_file = NULL;
	sr_xpath_ctx_t state = {0};
	char *tmp_xpath = NULL;

	tmp_xpath = xstrdup(xpath);

	user_node = sr_xpath_node_name((char *) xpath);
	user_name = sr_xpath_key_value((char *) xpath, "user", "name", &state);
	user_ssh_file_name = sr_xpath_key_value((char *) tmp_xpath, "authorized-key", "name", &state);

	if (user_ssh_file_name == NULL) {
		if (strcmp(user_node, "name") == 0) {
			// don't set empty string as a new user
			// only happens when you delete a user
			if (strcmp(value, "") == 0) {
				FREE_SAFE(tmp_xpath);
				return 0;
			}
			error = local_user_add_user(user_list, value);
			if (error != 0) {
				SRP_LOG_ERRMSG("local_user_add_user error");
				return -1;
			}
		} else if (strcmp(user_node, "password") == 0) {
			error = local_user_set_password(user_list, user_name, value);
			if (error != 0) {
				SRP_LOG_ERRMSG("local_user_set_password error");
				return -1;
			}
		}
	} else {
		// check if ssh public key file has .pub extension
		if (!has_pub_extension(user_ssh_file_name)) {
			// if it doesn't, add it
			size_t ssh_file_len = 0;

			ssh_file_len = strlen(user_ssh_file_name) + 5; // ".pub" + "\0"
			tmp_ssh_file = xmalloc(ssh_file_len);
			snprintf(tmp_ssh_file, ssh_file_len, "%s.pub", user_ssh_file_name);

			user_ssh_file_name = tmp_ssh_file;
		}

		if (strcmp(user_node, "name") == 0) {
			error = local_user_add_key(user_list, user_name, user_ssh_file_name);
			if (error != 0) {
				SRP_LOG_ERRMSG("local_user_add_key error");
				return -1;
			}
		} else if (strcmp(user_node, "algorithm") == 0) {
			error = local_user_add_algorithm(user_list, user_name, user_ssh_file_name, value);
			if (error != 0) {
				SRP_LOG_ERRMSG("local_user_add_algorithm error");
				return -1;
			}
		} else if (strcmp(user_node, "key-data") == 0) {
			error = local_user_add_key_data(user_list, user_name, user_ssh_file_name, value);
			if (error != 0) {
				SRP_LOG_ERRMSG("local_user_add_key_data error");
				return -1;
			}
		}
	}

	FREE_SAFE(tmp_xpath);

	if (tmp_ssh_file != NULL) {
		// in case there was no .pub extension
		// user_ssh_file_name points to tmp_ssh_file
		FREE_SAFE(user_ssh_file_name);
	}

	return 0;
}

static int set_ntp(const char *xpath, char *value)
{
	int error = 0;

	if (strcmp(xpath, NTP_ENABLED_YANG_PATH) == 0) {
		if (strcmp(value, "true") == 0){
			// TODO: replace "system()" call with sd-bus later if needed
			error = system("systemctl enable --now ntpd");
			if (error != 0) {
				SRP_LOG_ERR("\"systemctl enable --now ntpd\" failed with return value: %d", error);
				/* Debian based systems have a service file named
				 * ntp.service instead of ntpd.service for some reason...
				 */
				SRP_LOG_ERRMSG("trying systemctl enable --now ntp instead");
				error = system("systemctl enable --now ntp");
				if (error != 0) {
					SRP_LOG_ERR("\"systemctl enable --now ntp\" failed with return value: %d", error);
					return -1;
				}
			}
			// TODO: check if ntpd was enabled
		} else if (strcmp(value, "false") == 0) {
			// TODO: add - 'systemctl stop ntpd' as well ?
			error = system("systemctl stop ntpd");
			if (error != 0) {
				SRP_LOG_ERR("\"systemctl stop ntpd\" failed with return value: %d", error);
				/* Debian based systems have a service file named
				 * ntp.service instead of ntpd.service for some reason...
				 */
				SRP_LOG_ERRMSG("trying systemctl stop ntp instead");
				error = system("systemctl stop ntp");
				if (error != 0) {
					SRP_LOG_ERR("\"systemctl stop ntp\" failed with return value: %d", error);
					return -1;
				} else {
					error = system("systemctl disable ntp");
					if (error != 0) {
						SRP_LOG_ERR("\"systemctl disable ntpd\" failed with return value: %d", error);
						return -1;
					}
				}
			} else {
				error = system("systemctl disable ntpd");
				if (error != 0) {
					SRP_LOG_ERR("\"systemctl disable ntpd\" failed with return value: %d", error);
					return -1;
				}
			}
			// TODO: check if ntpd was disabled
		}
	} else if (strncmp(xpath, NTP_SERVER_YANG_PATH, strlen(NTP_SERVER_YANG_PATH)) == 0) {
		char *ntp_node = NULL;
		char *ntp_server_name = NULL;
		sr_xpath_ctx_t state = {0};

		ntp_node = sr_xpath_node_name((char *) xpath);
		ntp_server_name = sr_xpath_key_value((char *) xpath, "server", "name", &state);

		if (strcmp(ntp_node, "name") == 0) {
			if (strcmp(value, "") == 0){
				error = ntp_server_list_set_delete(ntp_servers, ntp_server_name, true);
				if (error != 0) {
					SRP_LOG_ERRMSG("ntp_server_list_set_delete error");
					return -1;
				}
			} else {
				error = ntp_server_list_add_server(ntp_servers, value);
				if (error != 0) {
					SRP_LOG_ERRMSG("error adding new ntp server");
					return -1;
				}
			}

		} else if (strcmp(ntp_node, "address") == 0) {
			error = ntp_server_list_set_address(ntp_servers, ntp_server_name, value);
			if (error != 0) {
				SRP_LOG_ERRMSG("error setting ntp server address");
				return -1;
			}
			// set the address and name to a file
			error = ntp_set_server_name(value, ntp_server_name);
			if (error != 0) {
				SRP_LOG_ERRMSG("ntp_set_server_name error");
				return -1;
			}

		} else if (strcmp(ntp_node, "port") == 0) {
			error = ntp_server_list_set_port(ntp_servers, ntp_server_name, value);
			if (error != 0) {
				SRP_LOG_ERRMSG("error setting ntp server port");
				return -1;
			}

		} else if (strcmp(ntp_node, "association-type") == 0) {
			error = ntp_server_list_set_assoc_type(ntp_servers, ntp_server_name, value);
			if (error != 0) {
				SRP_LOG_ERRMSG("error setting ntp server association-type");
				return -1;
			}

		} else if (strcmp(ntp_node, "iburst") == 0) {
			if (strcmp(value, "true") == 0) {
				error = ntp_server_list_set_iburst(ntp_servers, ntp_server_name, "iburst");
				if (error != 0) {
					SRP_LOG_ERRMSG("error setting ntp server iburst");
					return -1;
				}
			} else {
				error = ntp_server_list_set_iburst(ntp_servers, ntp_server_name, "");
				if (error != 0) {
					SRP_LOG_ERRMSG("error setting ntp server iburst");
					return -1;
				}
			}

		} else if (strcmp(ntp_node, "prefer") == 0) {
			if (strcmp(value, "true") == 0) {
				error = ntp_server_list_set_prefer(ntp_servers, ntp_server_name, "prefer");
				if (error != 0) {
					SRP_LOG_ERRMSG("error setting ntp server prefer");
					return -1;
				}
			} else {
				error = ntp_server_list_set_prefer(ntp_servers, ntp_server_name, "");
				if (error != 0) {
					SRP_LOG_ERRMSG("error setting ntp server prefer");
					return -1;
				}
			}
		}
	}

	return 0;
}

static int set_dns(const char *xpath, char *value, sr_change_oper_t operation)
{
	int err = 0;
	char *nn = sr_xpath_node_name(xpath);

	SRP_LOG_DBG("Xpath for dns-resolver: %s -> %s = %s", xpath, nn, value);

	// first check for server -> if not server change then watch for other leafs/leaf lists
	if (strncmp(xpath, DNS_RESOLVER_SERVER_YANG_PATH, sizeof(DNS_RESOLVER_SERVER_YANG_PATH) - 1) == 0) {
		char *name = NULL;
		sr_xpath_ctx_t state = {0};

		name = sr_xpath_key_value((char *) xpath, "server", "name", &state);

		if (strcmp(nn, "name") == 0) {
			if (operation == SR_OP_CREATED) {
				SRP_LOG_DBG("Creating server '%s'", value);
				err = dns_server_list_add_server(&dns_servers, value);
			} else if (operation == SR_OP_DELETED) {
				SRP_LOG_DBG("deleting server '%s'", value);
				err = dns_server_list_set_server_delete(&dns_servers, value);
			}
		} else if (strcmp(nn, "address") == 0) {
			// set server name
			SRP_LOG_DBG("Setting server %s address to '%s'", name, value);
			err = dns_server_list_set_address(&dns_servers, name, value);
		} else if (strcmp(nn, "port") == 0) {
			// set server port
			err = dns_server_list_set_port(&dns_servers, name, atoi(value));
		}
	} else if (strcmp(nn, "search") == 0) {
		switch (operation) {
			case SR_OP_CREATED:
				SRP_LOG_DBG("Adding dns-resolver 'search' = '%s'", value);
				err = dns_search_add(value);
				break;
			case SR_OP_MODIFIED:
				break;
			case SR_OP_DELETED:
				SRP_LOG_DBG("Removing dns-resolver 'search' = '%s'", value);
				err = dns_search_remove(value);
				break;
			case SR_OP_MOVED:
				break;
		}
	} else if (strcmp(nn, "timeout") == 0) {
#ifdef SYSTEMD
		// unknown for systemd
		SRP_LOG_ERRMSG("Unsupported option 'timeout'... Aborting...");
		err = -1;
#else
		SRP_LOG_DBGMSG("Setting DNS timeout value...");
		err = set_dns_timeout(value);
#endif
	} else if (strcmp(nn, "attempts") == 0) {
#ifdef SYSTEMD
		// unknown for systemd
		SRP_LOG_ERRMSG("Unsupported option 'attempts'... Aborting...");
		err = -1;
#else
		SRP_LOG_DBGMSG("Setting DNS attempts value...");
		err = set_dns_attempts(value);
#endif
	}
	return err;
}

#ifndef SYSTEMD
static int set_dns_timeout(char *value)
{
	int err = 0;
	// load config and set timeout option in it
	rconf_t cfg;
	rconf_error_t rc_err = rconf_error_none;
	int timeout = 0;
	rconf_init(&cfg);

	rc_err = rconf_load_file(&cfg, RESOLV_CONF_PATH);
	if (rc_err != rconf_error_none) {
		goto err_out;
	}

	timeout = atoi(value);
	SRP_LOG_DBG("New timeout value: %d", timeout);

	rc_err = rconf_set_timeout(&cfg, timeout);
	if (rc_err) {
		goto err_out;
	}

	rc_err = rconf_export(&cfg, RESOLV_CONF_PATH);
	if (rc_err) {
		goto err_out;
	}

	goto out;

err_out:
	SRP_LOG_ERR("Error occured with resolv.conf: (%d) -> %s\n", rc_err, rconf_error2str(rc_err));
out:
	rconf_free(&cfg);
	return err;
}

static int set_dns_attempts(char *value)
{
	int err = 0;
	// load config and set attempts option in it
	rconf_t cfg;
	rconf_error_t rc_err = rconf_error_none;
	int attempts = 0;
	rconf_init(&cfg);
	rc_err = rconf_load_file(&cfg, RESOLV_CONF_PATH);

	if (rc_err != rconf_error_none) {
		goto err_out;
	}

	attempts = atoi(value);
	SRP_LOG_DBG("New attempts value: %d", attempts);

	rc_err = rconf_set_attempts(&cfg, attempts);
	if (rc_err) {
		goto err_out;
	}

	rc_err = rconf_export(&cfg, RESOLV_CONF_PATH);
	if (rc_err) {
		goto err_out;
	}

	goto out;

err_out:
	SRP_LOG_ERR("Error occured with resolv.conf: (%d) -> %s\n", rc_err, rconf_error2str(rc_err));
out:
	rconf_free(&cfg);
	return err;
}
#endif

static int set_contact_info(const char *value)
{
	struct passwd *pwd = {0};
	FILE *tmp_pwf = NULL; // temporary passwd file
	int read_fd = -1;
	int write_fd = -1;
	struct stat stat_buf = {0};
	off_t offset = 0;

	// write /etc/passwd to a temp file
	// and change GECOS field for CONTACT_USERNAME
	tmp_pwf = fopen(CONTACT_TEMP_FILE, "w");
	if (!tmp_pwf) {
		goto fail;
	}

	endpwent(); // close the passwd db

	pwd = getpwent();
	if (pwd == NULL) {
		goto fail;
	}

	do {
		if (strcmp(pwd->pw_name, CONTACT_USERNAME) == 0) {
			// TODO: check max allowed len of gecos field
			pwd->pw_gecos = (char *) value;

			if (putpwent(pwd, tmp_pwf) != 0) {
				goto fail;
			}

		} else{
			if (putpwent(pwd, tmp_pwf) != 0) {
				goto fail;
			}
		}
	} while ((pwd = getpwent()) != NULL);

	fclose(tmp_pwf);
	tmp_pwf = NULL;

	// create a backup file of /etc/passwd
	if (rename(PASSWD_FILE, PASSWD_BAK_FILE) != 0) {
		goto fail;
	}

	// copy the temp file to /etc/passwd
	read_fd = open(CONTACT_TEMP_FILE, O_RDONLY);
	if (read_fd == -1) {
		goto fail;
	}

	if (fstat(read_fd, &stat_buf) != 0) {
		goto fail;
	}

	write_fd = open(PASSWD_FILE, O_WRONLY | O_CREAT, stat_buf.st_mode);
	if (write_fd == -1) {
		goto fail;
	}

	if (sendfile(write_fd, read_fd, &offset, (size_t)stat_buf.st_size) == -1) {
		goto fail;
	}

	// remove the temp file
	if (remove(CONTACT_TEMP_FILE) != 0) {
		goto fail;
	}

	close(read_fd);
	close(write_fd);

	return 0;

fail:
	// if copying tmp file to /etc/passwd failed
	// rename the backup back to passwd
	if (access(PASSWD_FILE, F_OK) != 0 ) {
		rename(PASSWD_BAK_FILE, PASSWD_FILE);
	}

	if (tmp_pwf != NULL) {
		fclose(tmp_pwf);
	}

	if (read_fd != -1) {
		close(read_fd);
	}

	if (write_fd != -1) {
		close(write_fd);
	}
		
	return -1;
}

static int get_contact_info(char *value)
{
	struct passwd *pwd = {0};

	setpwent();

	pwd = getpwent();

	if (pwd == NULL) {
		return -1;
	}

	do {
		if (strcmp(pwd->pw_name, CONTACT_USERNAME) == 0) {
			strncpy(value, pwd->pw_gecos, strnlen(pwd->pw_gecos, MAX_GECOS_LEN));
		}
	} while ((pwd = getpwent()) != NULL);

	endpwent();

	return 0;
}

static int set_timezone(const char *value)
{
	int error = 0;
	char *zoneinfo = TIMEZONE_DIR; // not NULL terminated
	char *timezone = NULL;

	timezone = xmalloc(strnlen(zoneinfo, ZONE_DIR_LEN) + strnlen(value, TIMEZONE_NAME_LEN) + 1);

	strncpy(timezone, zoneinfo, strnlen(zoneinfo, ZONE_DIR_LEN) + 1);
	strncat(timezone, value, strnlen(value, TIMEZONE_NAME_LEN));

	// check if file exists in TIMEZONE_DIR
	if (access(timezone, F_OK) != 0) {
		goto fail;
	}

	if (access(LOCALTIME_FILE, F_OK) == 0) {
		// if the /etc/localtime symlink file exists
		// unlink it
		error = unlink(LOCALTIME_FILE);
		if (error != 0) {
			goto fail;
		}
	} // if it doesn't, it will be created

	error = symlink(timezone, LOCALTIME_FILE);
	if (error != 0)
		goto fail;

	FREE_SAFE(timezone);
	return 0;

fail:
	FREE_SAFE(timezone);
	return -1;
}

static int get_timezone_name(char *value)
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

static char *system_xpath_get(const struct lyd_node *node)
{
	char *xpath_node = NULL;
	char *xpath_leaflist_open_bracket = NULL;
	size_t xpath_trimed_size = 0;
	char *xpath_trimed = NULL;

	if (node->schema->nodetype == LYS_LEAFLIST) {
		xpath_node = lyd_path(node);
		xpath_leaflist_open_bracket = strrchr(xpath_node, '[');
		if (xpath_leaflist_open_bracket == NULL) {
			return xpath_node;
		}

		xpath_trimed_size = (size_t) xpath_leaflist_open_bracket - (size_t) xpath_node + 1;
		xpath_trimed = xcalloc(1, xpath_trimed_size);
		strncpy(xpath_trimed, xpath_node, xpath_trimed_size - 1);
		xpath_trimed[xpath_trimed_size - 1] = '\0';

		FREE_SAFE(xpath_node);

		return xpath_trimed;
	} else {
		return lyd_path(node);
	}
}

static int system_state_data_cb(sr_session_ctx_t *session, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data)
{
	int error = SR_ERR_OK;
	// TODO: create struct that holds this
	//		 pass the struct to store_values_to_datastore
	//result_values_t *values = NULL;
	char *os_name = NULL;
	char *os_release = NULL;
	char *os_version = NULL;
	char *machine = NULL;
	char current_datetime[DATETIME_BUF_SIZE] = {0};
	char boot_datetime[DATETIME_BUF_SIZE] = {0};

	error = get_os_info(&os_name, &os_release, &os_version, &machine);
	if (error) {
		SRP_LOG_ERR("get_os_info error: %s", strerror(errno));
		goto out;
	}

	error = get_datetime_info(current_datetime, boot_datetime);
	if (error) {
		SRP_LOG_ERR("get_datetime_info error: %s", strerror(errno));
		goto out;
	}

	/*error = store_values_to_datastore(session, request_xpath, values, parent);
	// TODO fix error handling here
	if (error) {
		SRP_LOG_ERR("store_values_to_datastore error (%d)", error);
		goto out;
	} */

	// TODO: replace this with the above call to store_values_to_datastore
	const struct ly_ctx *ly_ctx = NULL;
	if (*parent == NULL) {
		ly_ctx = sr_get_context(sr_session_get_connection(session));
		if (ly_ctx == NULL) {
			goto out;
		}
		*parent = lyd_new_path(NULL, ly_ctx, SYSTEM_STATE_YANG_MODEL, NULL, 0, 0);
	}

	lyd_new_path(*parent, NULL, OS_NAME_YANG_PATH, os_name, 0, 0);
	lyd_new_path(*parent, NULL, OS_RELEASE_YANG_PATH, os_release, 0, 0);
	lyd_new_path(*parent, NULL, OS_VERSION_YANG_PATH, os_version, 0, 0);
	lyd_new_path(*parent, NULL, OS_MACHINE_YANG_PATH, machine, 0, 0);

	lyd_new_path(*parent, NULL, CURR_DATETIME_YANG_PATH, current_datetime, 0, 0);
	lyd_new_path(*parent, NULL, BOOT_DATETIME_YANG_PATH, boot_datetime, 0, 0);

	//values = NULL;

out:
	if (os_name != NULL) {
		FREE_SAFE(os_name);
	}
	if (os_release != NULL) {
		FREE_SAFE(os_release);
	}
	if (os_version != NULL) {
		FREE_SAFE(os_version);
	}
	if (machine != NULL) {
		FREE_SAFE(machine);
	}

	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}
/*
static int store_values_to_datastore(sr_session_ctx_t *session, const char *request_xpath, result_values_t *values, struct lyd_node **parent)
{
	const struct ly_ctx *ly_ctx = NULL;
	if (*parent == NULL) {
		ly_ctx = sr_get_context(sr_session_get_connection(session));
		if (ly_ctx == NULL) {
			return -1;
		}
		*parent = lyd_new_path(NULL, ly_ctx, request_xpath, NULL, 0, 0);
	}

	for (size_t i = 0; i < values->num_values; i++) {
		lyd_new_path(*parent, NULL, values->values[i].xpath, values->values[i].value, 0, 0);
	}

	return 0;
}
*/

static int get_os_info(char **os_name, char **os_release, char **os_version, char **machine)
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

static int get_datetime_info(char current_datetime[], char boot_datetime[])
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

static int system_rpc_cb(sr_session_ctx_t *session, const char *op_path, const sr_val_t *input, const size_t input_cnt, sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data)
{
	int error = SR_ERR_OK;
	char *datetime = NULL;

	if (strcmp(op_path, SET_CURR_DATETIME_YANG_PATH) == 0) {
		if (input_cnt != 1) {
			SRP_LOG_ERRMSG("system_rpc_cb: input_cnt != 1");
			goto error_out;
		}

		datetime = input[0].data.string_val;

		error = set_datetime(datetime);
		if (error) {
			SRP_LOG_ERR("set_datetime error: %s", strerror(errno));
			goto error_out;
		}

		error = sr_set_item_str(session, CURR_DATETIME_YANG_PATH, datetime, NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRP_LOG_ERR("sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		error = sr_apply_changes(session, 0, 0);
		if (error) {
			SRP_LOG_ERR("sr_apply_changes error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		SRP_LOG_INFMSG("system_rpc_cb: CURR_DATETIME_YANG_PATH and system time successfully set!");

	} else if (strcmp(op_path, RESTART_YANG_PATH) == 0) {
		sync();
		system("shutdown -r");
		SRP_LOG_INFMSG("system_rpc_cb: restarting the system!");
	} else if (strcmp(op_path, SHUTDOWN_YANG_PATH) == 0) {
		sync();
		system("shutdown -P");
		SRP_LOG_INFMSG("system_rpc_cb: shutting down the system!");
	} else {
		SRP_LOG_ERR("system_rpc_cb: invalid path %s", op_path);
		goto error_out;
	}

	return SR_ERR_OK;

error_out:
	return SR_ERR_CALLBACK_FAILED;
}

static int set_datetime(char *datetime)
{
	struct tm t = {0};
	time_t time_to_set = 0;
	struct timespec stime = {0};

	/* datetime format must satisfy constraint:
		"\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(\.\d+)?(Z|[\+\-]\d{2}:\d{2})"
		currently only "%d-%d-%dT%d-%d-%dZ" is supported
		TODO: Add support for:
			- 2021-02-09T06:02:39.234+01:00
			- 2021-02-09T06:02:39.234Z
			- 2021-02-09T06:02:39+11:11
	*/

	if (strptime(datetime, "%FT%TZ", &t) == NULL) {
		return -1;
	}

	time_to_set = mktime(&t);
	if (time_to_set == -1) {
		return -1;
	}

	stime.tv_sec = time_to_set;

	if (clock_settime(CLOCK_REALTIME, &stime) == -1) {
		return -1;
	}

	return 0;
}

static int set_location(const char *location)
{
	long int error = -1;
	char *location_file_path = NULL;
	int fd = -1;
	size_t len = 0;

	location_file_path = get_plugin_file_path(LOCATION_FILENAME, false);
	if (location_file_path == NULL) {
		SRP_LOG_ERRMSG("set_location: couldn't get location file path");
		goto error_out;
	}

	fd = open(location_file_path, O_CREAT|O_WRONLY|O_TRUNC);
	if (fd == -1) {
		SRP_LOG_ERRMSG("set_location: couldn't open location file path");
		goto error_out;
	}

	len = strnlen(location, MAX_LOCATION_LENGTH);
		
	error = write(fd, location, len);
	if (error == -1) {
		SRP_LOG_ERRMSG("set_location: couldn't write to location file path");
		goto error_out;
	}

	error = close(fd);
	if (error == -1) {
		SRP_LOG_ERRMSG("set_location: couldn't close location file path");
		goto error_out;
	}

	fd = -1;

	FREE_SAFE(location_file_path);

	return 0;

error_out:
	if (location_file_path != NULL) {
		FREE_SAFE(location_file_path);
	}

	if (fd == -1) {
		close(fd);
	}

	return -1;
}

static int get_location(char *location)
{
	FILE *fp = NULL;
	char *location_file_path = NULL;

	location_file_path = get_plugin_file_path(LOCATION_FILENAME, false);
	if (location_file_path == NULL) {
		SRP_LOG_ERRMSG("get_location: couldn't get location file path");
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

int ntp_set_server_name(char *address, char *name)
{
	FILE *fp = NULL;
	FILE *fp_tmp = NULL;
	char entry[NTP_MAX_ENTRY_LEN] = {0};
	char *ntp_names_file_path = NULL;
	char *tmp_ntp_names_file_path = NULL;
	char *line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	bool entry_updated = false;

	ntp_names_file_path = get_plugin_file_path(NTP_NAMES_FILENAME, false);
	if (ntp_names_file_path == NULL) {
		SRP_LOG_ERRMSG("ntp_set_server_name: couldn't get ntp_names file path");
		goto error_out;
	}

	if (strcmp(address, "") != 0) { // leave entry empty if it needs to be deleted
		if (snprintf(entry, NTP_MAX_ENTRY_LEN, "%s=%s\n", name, address) < 0) {
			goto error_out;
		}
	}

	tmp_ntp_names_file_path = get_plugin_file_path(NTP_TMP_NAMES_FILENAME, true);
	if (ntp_names_file_path == NULL) {
		SRP_LOG_ERRMSG("ntp_set_server_name: couldn't get tmp_ntp_names file path");
		goto error_out;
	}

	fp = fopen(ntp_names_file_path, "r");
	if (fp == NULL) {
		goto error_out;
	}

	fp_tmp = fopen(tmp_ntp_names_file_path, "a");
	if (fp_tmp == NULL) {
		goto error_out;
	}

	while ((read = getline(&line, &len, fp)) != -1) {
		// check if ntp server with name already exists
		if (strncmp(line, name, strlen(name)) == 0) {
			// update it
			fputs(entry, fp_tmp);
			entry_updated = true;
		} else {
			fputs(line, fp_tmp);
		}
	}

	FREE_SAFE(line);
	fclose(fp);
	fp = NULL;
	fclose(fp_tmp);
	fp_tmp = NULL;

	// rename the tmp file
	if (rename(tmp_ntp_names_file_path, ntp_names_file_path) != 0) {
		goto error_out;
	}

	FREE_SAFE(tmp_ntp_names_file_path);

	// if the current entry wasn't updated, append it
	if (!entry_updated) {
		fp = fopen(ntp_names_file_path, "a");
		if (fp == NULL) {
			goto error_out;
		}

		fputs(entry, fp);

		fclose(fp);
		fp = NULL;
	}

	FREE_SAFE(ntp_names_file_path);
	return 0;

error_out:
	if (ntp_names_file_path != NULL) {
		FREE_SAFE(ntp_names_file_path);
	}

	if (tmp_ntp_names_file_path != NULL) {
		FREE_SAFE(tmp_ntp_names_file_path);
	}

	if (fp != NULL) {
		fclose(fp);
	}

	if (fp_tmp != NULL) {
		fclose(fp_tmp);
	}

	return -1;
}

int ntp_get_server_name(char **name, char *address)
{
	int error = 0;
	FILE *fp = NULL;
	char *line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	char *ntp_file_path = NULL;
	bool entry_found = false;

	ntp_file_path = get_plugin_file_path(NTP_NAMES_FILENAME, false);
	if (ntp_file_path == NULL) {
		SRP_LOG_ERRMSG("ntp_get_server_name: couldn't get ntp_names file path");
		goto error_out;
	}

	fp = fopen(ntp_file_path, "r");
	if (fp == NULL) {
		FREE_SAFE(ntp_file_path);
		goto error_out;
	}

	while ((read = getline(&line, &len, fp)) != -1) {
		// find name for given ntp server address
		if (strstr(line, address) != NULL) {
			// remove the newline char from line
			line[strlen(line) - 1] = '\0';

			char *tmp_name = strchr(line, '=');

			// "truncate" line buf by placing null term where '=' is
			*tmp_name = '\0';

			*name = xstrdup(line); // line contains the string before '=' now

			entry_found = true;

			break;
		}
	}

	if (!entry_found) {
		SRP_LOG_INF("No name in %s for ntp server with address %s was found", ntp_file_path, address);
		SRP_LOG_INF("Setting address %s as name...", address);

		*name = xstrdup(address);

		// save to file
		error = ntp_set_server_name(address, *name);
		if (error != 0) {
			SRP_LOG_ERRMSG("ntp_set_server_name error");
			goto error_out;
		}
	}

	FREE_SAFE(ntp_file_path);
	FREE_SAFE(line);
	fclose(fp);

	return 0;

error_out:
	if (ntp_file_path != NULL) {
		FREE_SAFE(ntp_file_path);
	}

	if (line != NULL) {
		FREE_SAFE(line);
	}

	if (fp != NULL) {
		fclose(fp);
	}

	return -1;
}
