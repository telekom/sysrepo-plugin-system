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

#include <sysrepo.h>

#include "utils/memory.h"
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

#define BASE_YANG_MODEL "ietf-system"
#define SYSTEM_YANG_MODEL "/" BASE_YANG_MODEL ":system"

#define SET_CURR_DATETIME_YANG_PATH "/" BASE_YANG_MODEL ":set-current-datetime"
#define RESTART_YANG_PATH "/" BASE_YANG_MODEL ":system-restart"
#define SHUTDOWN_YANG_PATH "/" BASE_YANG_MODEL ":system-shutdown"

#define CONTACT_YANG_PATH SYSTEM_YANG_MODEL  "/contact"
#define HOSTNAME_YANG_PATH SYSTEM_YANG_MODEL "/hostname"
#define LOCATION_YANG_PATH SYSTEM_YANG_MODEL "/location"

#define CLOCK_YANG_PATH SYSTEM_YANG_MODEL 	 "/clock"
#define TIMEZONE_NAME_YANG_PATH CLOCK_YANG_PATH "/timezone-name"
#define TIMEZONE_OFFSET_YANG_PATH CLOCK_YANG_PATH "/timezone-utc-offset"

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
#define PASSWD_FILE "/etc/passwd"
#define PASSWD_BAK_FILE PASSWD_FILE ".bak"

#define TIMEZONE_DIR "/usr/share/zoneinfo/"
#define LOCALTIME_FILE "/etc/localtime"
#define ZONE_DIR_LEN 20 // '/usr/share/zoneinfo' length
#define TIMEZONE_NAME_LEN 14*3 // The Area and Location names have a maximum length of 14 characters, but areas can have a subarea

#define DATETIME_BUF_SIZE 30
#define UTS_LEN 64

static int system_module_change_cb(sr_session_ctx_t *session, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);
static int system_state_data_cb(sr_session_ctx_t *session, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);
static int system_rpc_cb(sr_session_ctx_t *session, const char *op_path, const sr_val_t *input, const size_t input_cnt, sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data);

int set_config_value(const char *xpath, const char *value );
int set_contact_info(const char *value);
int set_timezone(const char *value);
static char *system_xpath_get(const struct lyd_node *node);

int get_os_info(char **os_name, char **os_release, char **os_version, char **machine);
int get_datetime_info(char current_datetime[], char boot_datetime[]);

int set_datetime(char *datetime);

int sr_plugin_init_cb(sr_session_ctx_t *session, void **private_data)
{
	int error = 0;
	sr_conn_ctx_t *connection = NULL;
	sr_session_ctx_t *startup_session = NULL;
	sr_subscription_ctx_t *subscription = NULL;

	*private_data = NULL;

	SRP_LOG_INFMSG("start session to startup datastore");

	connection = sr_session_get_connection(session);
	error = sr_session_start(connection, SR_DS_STARTUP, &startup_session);
	if (error) {
		SRP_LOG_ERR("sr_session_start error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	*private_data = startup_session;

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

	goto out;

error_out:
	sr_unsubscribe(subscription);

out:
	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

void sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *private_data)
{
	sr_session_ctx_t *startup_session = (sr_session_ctx_t *) private_data;

	if (startup_session) {
		sr_session_stop(startup_session);
	}

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

			if (node->schema->nodetype == LYS_LEAF) {
				if (operation == SR_OP_CREATED || operation == SR_OP_MODIFIED) {
					error = set_config_value(node_xpath, node_value);
					if (error) {
						SRP_LOG_ERR("set_config_value error (%d)", error);
						goto error_out;
					}
				} else if (operation == SR_OP_DELETED) {
					// TODO: discussion with DT needed
				}
			}
			FREE_SAFE(node_xpath);
			node_value = NULL;
		}
	}
	goto out;

error_out:
	//TODO: handle errors here
	//srpo_uci_revert("dhcp"); <- this won't be used
	//srpo_uci_revert("network"); <- this won't be used

out:
	FREE_SAFE(node_xpath);
	sr_free_change_iter(system_change_iter);

	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

int set_config_value(const char *xpath, const char *value)
{
	int error = 0;

	if (strcmp(xpath, HOSTNAME_YANG_PATH) == 0) {
		error = sethostname(value, strnlen(value, HOST_NAME_MAX));
		if (error != 0) {
			SRP_LOG_ERR("sethostname error: %s", strerror(errno));
		}
	} else if (strcmp(xpath, CONTACT_YANG_PATH) == 0) {
		error = set_contact_info(value);
		if (error != 0) {
			SRP_LOG_ERR("set_contact_info error: %s", strerror(errno));
		}
	} else if (strcmp(xpath, LOCATION_YANG_PATH) == 0) {
		/*	TODO: Add later...

			"The system location.
			A server implementation MAY map this leaf to the sysLocation
			MIB object.  Such an implementation needs to use some
			mechanism to handle the differences in size and characters
			allowed between this leaf and sysLocation.  The definition
			of such a mechanism is outside the scope of this document.";

			sysLocation
			"The physical location of this node (e.g., 'telephone
			closet, 3rd floor').  If the location is unknown, the
			value is the zero-length string."
		*/

	} else if (strcmp(xpath, TIMEZONE_NAME_YANG_PATH) == 0) {
		error = set_timezone(value);
		if (error != 0) {
			SRP_LOG_ERR("set_timezone error: %s", strerror(errno));
		}
	} else if (strcmp(xpath, TIMEZONE_OFFSET_YANG_PATH) == 0) {
		// timezone-utc-offset leaf
		// https://linux.die.net/man/5/tzfile
		// https://linux.die.net/man/8/zic
	}

	return error;
}

int set_contact_info(const char *value)
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
	if (!tmp_pwf)
		goto fail;

	while ((pwd = getpwent()) != NULL) {
		if (strcmp(pwd->pw_name, CONTACT_USERNAME) == 0) {
			// TODO: check max allowed len of gecos field
			pwd->pw_gecos = (char *)value;

			if (putpwent(pwd, tmp_pwf) != 0)
				goto fail;

		} else{
			if (putpwent(pwd, tmp_pwf) != 0)
				goto fail;
		}
	}

	fclose(tmp_pwf);
	tmp_pwf = NULL;

	// create a backup file of /etc/passwd
	if (rename(PASSWD_FILE, PASSWD_BAK_FILE) != 0)
		goto fail;

	// copy the temp file to /etc/passwd
	read_fd = open(CONTACT_TEMP_FILE, O_RDONLY);
	if (read_fd == -1)
		goto fail;

	if (fstat(read_fd, &stat_buf) != 0)
		goto fail;

	write_fd = open(PASSWD_FILE, O_WRONLY | O_CREAT, stat_buf.st_mode);
	if (write_fd == -1)
		goto fail;

	if (sendfile(write_fd, read_fd, &offset, (size_t)stat_buf.st_size) == -1)
		goto fail;

	// remove the temp file
	if (remove(CONTACT_TEMP_FILE) != 0)
		goto fail;

	close(read_fd);
	close(write_fd);

	return 0;

fail:
	// if copying tmp file to /etc/passwd failed
	// rename the backup back to passwd
	if (access(PASSWD_FILE, F_OK) != 0 )
		rename(PASSWD_BAK_FILE, PASSWD_FILE);

	if (tmp_pwf != NULL)
		fclose(tmp_pwf);

	if (read_fd != -1)
		close(read_fd);

	if (write_fd != -1)
		close(write_fd);
		
	return -1;
}

int set_timezone(const char *value)
{
	int error = 0;
	char *zoneinfo = TIMEZONE_DIR; // not NULL terminated
	char *timezone = NULL;

	timezone = xmalloc(strnlen(zoneinfo, ZONE_DIR_LEN) + strnlen(value, TIMEZONE_NAME_LEN) + 1);

	strncpy(timezone, zoneinfo, strnlen(zoneinfo, ZONE_DIR_LEN) + 1);
	strncat(timezone, value, strnlen(value, TIMEZONE_NAME_LEN));

	// check if file exists in TIMEZONE_DIR
	if (access(timezone, F_OK) != 0 )
		goto fail;

	error = unlink(LOCALTIME_FILE);
	if (error != 0) {
		goto fail;
	}

	error = symlink(timezone, LOCALTIME_FILE);
	if (error != 0)
		goto fail;

	FREE_SAFE(timezone);
	return 0;

fail:
	FREE_SAFE(timezone);
	return -1;
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
			return -1;
		}
		*parent = lyd_new_path(NULL, ly_ctx, request_xpath, NULL, 0, 0);
	}

	lyd_new_path(*parent, NULL, OS_NAME_YANG_PATH, os_name, 0, 0);
	lyd_new_path(*parent, NULL, OS_RELEASE_YANG_PATH, os_release, 0, 0);
	lyd_new_path(*parent, NULL, OS_VERSION_YANG_PATH, os_version, 0, 0);
	lyd_new_path(*parent, NULL, OS_MACHINE_YANG_PATH, machine, 0, 0);

	lyd_new_path(*parent, NULL, CURR_DATETIME_YANG_PATH, current_datetime, 0, 0);
	lyd_new_path(*parent, NULL, BOOT_DATETIME_YANG_PATH, boot_datetime, 0, 0);

	//values = NULL;
	FREE_SAFE(os_name);
	FREE_SAFE(os_release);
	FREE_SAFE(os_version);
	FREE_SAFE(machine);

	return SR_ERR_OK;

out:
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

int get_os_info(char **os_name, char **os_release, char **os_version, char **machine){
	int error = 0;
	struct utsname uname_data = {0};

	if (uname(&uname_data) < 0)
		error = -1;

	*os_name = xmalloc(strnlen(uname_data.sysname, UTS_LEN + 1));
	*os_release = xmalloc(strnlen(uname_data.release, UTS_LEN + 1));
	*os_version = xmalloc(strnlen(uname_data.version, UTS_LEN + 1));
	*machine = xmalloc(strnlen(uname_data.machine, UTS_LEN + 1));

	strncpy(*os_name, uname_data.sysname, strnlen(uname_data.sysname, UTS_LEN + 1));
	strncpy(*os_release, uname_data.release, strnlen(uname_data.release, UTS_LEN + 1));
	strncpy(*os_version, uname_data.version, strnlen(uname_data.version, UTS_LEN + 1));
	strncpy(*machine, uname_data.machine, strnlen(uname_data.machine, UTS_LEN + 1));

	return error;
}

int get_datetime_info(char current_datetime[], char boot_datetime[])
{
	time_t now = 0;
	struct tm *ts = {0};
	struct sysinfo s_info = {0};
	time_t uptime_seconds = 0;

	now = time(NULL);

	ts = localtime(&now);
	if (ts == NULL)
		return -1;

	/* must satisfy constraint:
		"\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(\.\d+)?(Z|[\+\-]\d{2}:\d{2})"
		TODO: Add support for:
			- 2021-02-09T06:02:39.234+01:00
			- 2021-02-09T06:02:39.234Z
			- 2021-02-09T06:02:39+11:11
	*/

	strftime(current_datetime, DATETIME_BUF_SIZE, "%FT%TZ", ts);

	if (sysinfo(&s_info) != 0)
		return -1;

	uptime_seconds = s_info.uptime;

	time_t diff = now - uptime_seconds;

	ts = localtime(&diff);
	if (ts == NULL)
		return -1;

	strftime(boot_datetime, DATETIME_BUF_SIZE, "%FT%TZ", ts);

	return 0;
}

static int system_rpc_cb(sr_session_ctx_t *session, const char *op_path, const sr_val_t *input, const size_t input_cnt, sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data)
{
	int error = SR_ERR_OK;
	char *datetime = NULL;

	if (strcmp(op_path, SET_CURR_DATETIME_YANG_PATH) == 0) {
		if (input_cnt != 1) {
			SRP_LOG_ERR("system_rpc_cb: input_cnt != 1");
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

int set_datetime(char *datetime)
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

	if (strptime(datetime, "%FT%TZ", &t) == NULL)
		return -1;

	time_to_set = mktime(&t);
	if (time_to_set == -1)
		return -1;

	stime.tv_sec = time_to_set;

	if (clock_settime(CLOCK_REALTIME, &stime) == -1)
		return -1;

	return 0;
}

#ifndef PLUGIN
#include <signal.h>
#include <unistd.h>

volatile int exit_application = 0;

static void sigint_handler(__attribute__((unused)) int signum);

int main(void)
{
	int error = SR_ERR_OK;
	sr_conn_ctx_t *connection = NULL;
	sr_session_ctx_t *session = NULL;
	void *private_data = NULL;

	sr_log_stderr(SR_LL_DBG);

	/* connect to sysrepo */
	error = sr_connect(SR_CONN_DEFAULT, &connection);
	if (error) {
		SRP_LOG_ERR("sr_connect error (%d): %s", error, sr_strerror(error));
		goto out;
	}

	error = sr_session_start(connection, SR_DS_RUNNING, &session);
	if (error) {
		SRP_LOG_ERR("sr_session_start error (%d): %s", error, sr_strerror(error));
		goto out;
	}

	error = sr_plugin_init_cb(session, &private_data);
	if (error) {
		SRP_LOG_ERRMSG("sr_plugin_init_cb error");
		goto out;
	}

	/* loop until ctrl-c is pressed / SIGINT is received */
	signal(SIGINT, sigint_handler);
	signal(SIGPIPE, SIG_IGN);
	while (!exit_application) {
		sleep(1);
	}

out:
	sr_plugin_cleanup_cb(session, private_data);
	sr_disconnect(connection);

	return error ? -1 : 0;
}

static void sigint_handler(__attribute__((unused)) int signum)
{
	SRP_LOG_INFMSG("Sigint called, exiting...");
	exit_application = 1;
}

#endif
