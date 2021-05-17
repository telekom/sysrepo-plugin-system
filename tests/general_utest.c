#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "general.c"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

FILE *__real_fopen(const char *pathname, const char *mode);
struct passwd *__real_getpwent(void);
int __real_open(const char *pathname, int flags, mode_t mode);
char *__real_getenv(const char *name);

static void test_correct_set_datetime(void **state);
static void test_incorrect_set_datetime(void **state);
static void test_correct_get_datetime_info(void **state);
static void test_incorrect_get_datetime_info(void **state);
static void test_correct_get_os_info(void **state);
static void test_incorrect_get_os_info(void **state);
static void test_correct_get_timezone_name(void **state);
static void test_incorrect_get_timezone_name(void **state);
static void test_readlink_fail_get_timezone_name(void **state);
static void test_correct_get_plugin_file_path(void **state);
static void test_getenv_fail_get_plugin_file_path(void **state);
static void test_access_fail_get_plugin_file_path(void **state);
static void test_fopen_fail_get_plugin_file_path(void **state);
static void test_correct_get_contact_info(void **state);
static void test_incorrect_get_contact_info(void **state);
static void test_correct_set_contact_info(void **state);
static void test_incorrect_set_contact_info(void **state);
static void test_correct_get_location(void **state);
static void test_incorrect_get_location(void **state);
static void test_correct_set_location(void **state);
static void test_incorrect_set_location(void **state);

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_correct_set_datetime),
		cmocka_unit_test(test_incorrect_set_datetime),
		cmocka_unit_test(test_correct_get_datetime_info),
		cmocka_unit_test(test_incorrect_get_datetime_info),
		cmocka_unit_test(test_correct_get_os_info),
		cmocka_unit_test(test_incorrect_get_os_info),
		cmocka_unit_test(test_correct_get_timezone_name),
		cmocka_unit_test(test_incorrect_get_timezone_name),
		cmocka_unit_test(test_readlink_fail_get_timezone_name),
		cmocka_unit_test(test_correct_get_plugin_file_path),
		cmocka_unit_test(test_getenv_fail_get_plugin_file_path),
		cmocka_unit_test(test_access_fail_get_plugin_file_path),
		cmocka_unit_test(test_fopen_fail_get_plugin_file_path),
		cmocka_unit_test(test_correct_get_contact_info),
		cmocka_unit_test(test_incorrect_get_contact_info),
		cmocka_unit_test(test_correct_set_contact_info),
		cmocka_unit_test(test_incorrect_set_contact_info),
		cmocka_unit_test(test_correct_get_location),
		cmocka_unit_test(test_incorrect_get_location),
		cmocka_unit_test(test_correct_set_location),
		cmocka_unit_test(test_incorrect_set_location)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}

int __wrap_clock_settime(clockid_t clock_id, const struct timespec *tp)
{
	check_expected(clock_id);
	return (int) mock();
}


static void test_correct_set_datetime(void **state)
{
	(void) state;
	int rc = 0;

	expect_value(__wrap_clock_settime, clock_id, CLOCK_REALTIME);
	will_return(__wrap_clock_settime, 0);
	rc = set_datetime("2021-02-09T09:02:39Z");
	assert_int_equal(rc, 0);

	return;
}

static void test_incorrect_set_datetime(void **state)
{
	(void) state;
	int rc = 0;
	struct {
		char *datetime;
		int expected_rc;
	} incorrect_table[] = {
		// Correct but currently unsupported
		{.datetime = "2021-02-09T06:02:39.234+01:00", .expected_rc = -1},
		{.datetime = "2021-02-09T06:02:39.234Z", .expected_rc = -1},
		{.datetime = "2021-02-09T06:02:39+11:11", .expected_rc = -1},
		// incorrect
		{.datetime = "AAAA-02-09T06:02:39.234Z", .expected_rc = -1},
		{.datetime = "210392019302193", .expected_rc = -1},
		{.datetime = "---------", .expected_rc = -1},
	};

	for (size_t i = 0; i < ARRAY_SIZE(incorrect_table); i++) {
		rc = set_datetime(incorrect_table[i].datetime);
		assert_int_equal(rc, incorrect_table[i].expected_rc);
	}


}

static void test_correct_get_datetime_info(void **state)
{
	(void) state;
	char current_datetime[DATETIME_BUF_SIZE] = {0};
	char boot_datetime[DATETIME_BUF_SIZE] = {0};
	int rc = 0;

	will_return(__wrap_time, 1615406419);
	will_return(__wrap_sysinfo, 1000);
	will_return(__wrap_sysinfo, 0);
	rc = get_datetime_info(current_datetime, boot_datetime);
	assert_int_equal(rc, 0);
	assert_string_equal(current_datetime, "2021-03-10T21:00:19Z");
	assert_string_equal(boot_datetime, "2021-03-10T20:43:39Z");
}

static void test_incorrect_get_datetime_info(void **state)
{
	(void) state;
	char current_datetime[DATETIME_BUF_SIZE] = {0};
	char boot_datetime[DATETIME_BUF_SIZE] = {0};
	int rc = 0;

	will_return(__wrap_time, 1615406419);
	will_return(__wrap_sysinfo, 0);
	will_return(__wrap_sysinfo, -1);
	// Test handling of sysinfo error
	rc = get_datetime_info(current_datetime, boot_datetime);
	assert_int_equal(rc, -1);
}

time_t __wrap_time(time_t *tloc)
{
	return (time_t) mock();
}

int __wrap_sysinfo(struct sysinfo *info)
{
	info->uptime = (time_t) mock();
	return (int) mock();
}

static void test_correct_get_os_info(void **state)
{
	(void) state;
	char *os_name = NULL;
	char *os_release = NULL;
	char *os_version = NULL;
	char *machine = NULL;
	const char *os_name_expected = "OS_NAME_TEST";
	const char *os_release_expected = "OS_RELEASE_TEST";
	const char *os_version_expected = "OS_VERSION_TEST";
	const char *machine_expected = "MACHINE_TEST";
	int rc = 0;

	will_return(__wrap_uname, os_name_expected);
	will_return(__wrap_uname, os_release_expected);
	will_return(__wrap_uname, os_version_expected);
	will_return(__wrap_uname, machine_expected);
	will_return(__wrap_uname, 0);
	rc = get_os_info(&os_name, &os_release, &os_version, &machine);
	assert_int_equal(rc, 0);
	assert_string_equal(os_name, os_name_expected);
	assert_string_equal(os_release, os_release_expected);
	assert_string_equal(os_version, os_version_expected);
	assert_string_equal(machine, machine_expected);
}

static void test_incorrect_get_os_info(void **state)
{
	(void) state;
	char *os_name = NULL;
	char *os_release = NULL;
	char *os_version = NULL;
	char *machine = NULL;
	const char *os_name_expected = "OS_NAME_TEST";
	const char *os_release_expected = "OS_RELEASE_TEST";
	const char *os_version_expected = "OS_VERSION_TEST";
	const char *machine_expected = "MACHINE_TEST";
	int rc = 0;

	will_return(__wrap_uname, os_name_expected);
	will_return(__wrap_uname, os_release_expected);
	will_return(__wrap_uname, os_version_expected);
	will_return(__wrap_uname, machine_expected);
	will_return(__wrap_uname, -1);
	rc = get_os_info(&os_name, &os_release, &os_version, &machine);
	assert_int_equal(rc, -1);
	assert_null(os_name);
	assert_null(os_release);
	assert_null(os_version);
	assert_null(machine);
}

int __wrap_uname(struct utsname *buf)
{
	char *tmp = NULL;

	tmp = (char *) mock();
	strncpy(buf->sysname, tmp, strlen(tmp));
	tmp = (char *) mock();
	strncpy(buf->release, tmp, strlen(tmp));
	tmp = (char *) mock();
	strncpy(buf->version, tmp, strlen(tmp));
	tmp = (char *) mock();
	strncpy(buf->machine, tmp, strlen(tmp));

	return (int) mock();
}

static void test_correct_get_timezone_name(void **state)
{
	(void) state;
	char timezone_name[TIMEZONE_NAME_LEN] = {0};
	const char *timezone_path = "/usr/share/zoneinfo/Europe/Stockholm";
	const char *expected_timezone = "Europe/Stockholm";
	int rc = 0;

	will_return(__wrap_readlink, timezone_path);
	will_return(__wrap_readlink, strlen(timezone_path));
	rc = get_timezone_name(timezone_name);
	assert_int_equal(rc, 0);
	assert_string_equal(timezone_name, expected_timezone);
}

static void test_incorrect_get_timezone_name(void **state)
{
	(void) state;
	char timezone_name[TIMEZONE_NAME_LEN] = "This shouldn't change";
	const char *timezone_path = "/usr/share/error/Europe/Stockholm";
	int rc = 0;

	will_return(__wrap_readlink, timezone_path);
	will_return(__wrap_readlink, strlen(timezone_path));
	rc = get_timezone_name(timezone_name);
	assert_int_equal(rc, -1);
	assert_string_equal(timezone_name, "This shouldn't change");
}

static void test_readlink_fail_get_timezone_name(void **state)
{
	(void) state;
	char timezone_name[TIMEZONE_NAME_LEN] = "This shouldn't change";
	const char *timezone_path = "/usr/share/error/Europe/Stockholm";
	int rc = 0;

	will_return(__wrap_readlink, timezone_path);
	will_return(__wrap_readlink, -1);
	rc = get_timezone_name(timezone_name);
	assert_int_equal(rc, -1);
	assert_string_equal(timezone_name, "This shouldn't change");

}

ssize_t __wrap_readlink(const char *pathname, char *buf, size_t bufsize)
{
	const char *target = NULL;
	size_t min = bufsize;
	int error = 0;

	target = (const char *) mock();
	error = (int) mock();

	if (error < 0) {
		return error;
	}

	if (min > strlen(target)) {
		min = strlen(target);
	}

	memcpy(buf, target, min);
	
	return min;
}

static void test_correct_get_plugin_file_path(void **state)
{
	(void) state;
	const char *filename = "/file";
	bool create = false;

	char *expected_getenv = "/tmp";
		
	const char *expected_file_path = "/tmp/file";
	char *file_path;

	will_return(__wrap_getenv, expected_getenv);
	will_return(__wrap_access, 0);
	file_path = get_plugin_file_path(filename, create);
	assert_non_null(file_path);
	assert_string_equal(file_path, expected_file_path);
}

static void test_getenv_fail_get_plugin_file_path(void **state)
{
	(void) state;
	const char *filename = "/file";
	bool create = false;
	char *file_path;

	will_return(__wrap_getenv, NULL);
	file_path = get_plugin_file_path(filename, create);
	assert_null(file_path);
}

static void test_access_fail_get_plugin_file_path(void **state)
{
	(void) state;
	const char *filename = "/file";
	bool create = false;
	char *expected_getenv = "/somepath";
	char *file_path;

	will_return(__wrap_getenv, expected_getenv);
	will_return(__wrap_access, -1);
	file_path = get_plugin_file_path(filename, create);
	assert_null(file_path);
}

static void test_fopen_fail_get_plugin_file_path(void **state)
{
	(void) state;
	const char *filename = "/file";
	bool create = true;
	char *expected_getenv = "/somepath";
	char *file_path;

	will_return(__wrap_getenv, expected_getenv);
	will_return(__wrap_access, -1);
	will_return(__wrap_fopen, NULL);
	file_path = get_plugin_file_path(filename, create);
	assert_null(file_path);

}

char *__wrap_getenv(const char *name)
{	
	return (char *) mock();
}

int __wrap_access(const char *pathname, int mode)
{
	return (int) mock();
}

FILE *__wrap_fopen(const char *pathname, const char *mode)
{
	return (FILE *) mock();
}

static void test_correct_get_contact_info(void **state)
{
	(void) state;
	char value[MAX_GECOS_LEN] = "";

	struct passwd *pwd1 = (struct passwd *) malloc(sizeof(struct passwd));

	pwd1->pw_name = malloc(strlen(CONTACT_USERNAME) + 1);
	pwd1->pw_name = CONTACT_USERNAME;
	
	pwd1->pw_gecos = malloc(strlen("user1") + 1);
	pwd1->pw_gecos = "user1";

	int rc;

	will_return(__wrap_getpwent, pwd1);
	will_return(__wrap_getpwent, NULL);
	rc = get_contact_info(value);
	assert_int_equal(rc, 0);
	assert_string_equal(value, pwd1->pw_gecos);
}

struct passwd *__wrap_getpwent(void)
{
	return (struct passwd *) mock();
}

void __wrap_endpwent(void)
{
	function_called();
}

static void test_incorrect_get_contact_info(void **state)
{
	(void) state;
	char value[MAX_GECOS_LEN];
	int rc;


	will_return(__wrap_getpwent, NULL);
	rc = get_contact_info(value);
	assert_int_equal(rc, -1);
}

static void test_correct_set_contact_info(void **state)
{
	(void) state;
	const char *value = "user1";
	struct passwd pwd[] = {{.pw_name = "test", .pw_gecos = "testcontact",
			.pw_uid = 1001, .pw_gid = 1001, .pw_dir = "/home/test", .pw_shell = "/bin/bash",},
			{.pw_name = "root", .pw_gecos = "testcontact",
			.pw_uid = 0, .pw_gid = 0, .pw_dir = "/root", .pw_shell = "/bin/bash",},
			{.pw_name = "test2", .pw_gecos = "testcontact",
			.pw_uid = 1002, .pw_gid = 1002, .pw_dir = "/home/test2", .pw_shell = "/bin/bash",},
	};
	struct stat stat_buf = {0};	
	int rc = 0;
	FILE *fp = NULL;
	int read_fd = 0;
	int write_fd = 0;
#define BUF_SIZE 1000
	char buf[BUF_SIZE];
	const char *contact_path= "./tmp_passwd";
	const char *expected_passwd_path = "./passwd_result";
	const char *expected_passwd_lines[] = {"test::1001:1001:testcontact:/home/test:/bin/bash\n",
		"root::0:0:user1:/root:/bin/bash\n",
		"test2::1002:1002:testcontact:/home/test2:/bin/bash\n",
	};

	fp = __real_fopen(contact_path, "w");
	assert_non_null(fp);
	will_return(__wrap_fopen, fp);

	expect_function_call(__wrap_endpwent);

	will_return(__wrap_getpwent, &pwd[0]);
	will_return(__wrap_getpwent, &pwd[1]);
	will_return(__wrap_getpwent, &pwd[2]);
 	will_return(__wrap_getpwent, NULL);

	will_return(__wrap_rename, 0);

	read_fd = __real_open(contact_path, O_RDONLY, S_IRWXU);
	will_return(__wrap_open, read_fd);

	rc = fstat(read_fd, &stat_buf);
	assert_int_equal(rc, 0);

	write_fd = __real_open(expected_passwd_path, O_WRONLY | O_CREAT, stat_buf.st_mode);
	will_return(__wrap_open, write_fd);

	will_return(__wrap_remove, 0);

	rc = set_contact_info(value);
	assert_int_equal(rc, 0);

	fp = __real_fopen(expected_passwd_path, "r");
	assert_non_null(fp);

	for (size_t i = 0; i < ARRAY_SIZE(expected_passwd_lines); i++) {
		fgets(buf, BUF_SIZE - 1, fp);
		assert_string_equal(buf, expected_passwd_lines[i]);
	}

	fclose(fp);

#undef BUF_SIZE
}

static void test_incorrect_set_contact_info(void **state)
{
	(void) state;
	const char *value = "root";
	int rc;

	will_return(__wrap_fopen, NULL);
	will_return(__wrap_access, 0);
	rc = set_contact_info(value);
	assert_int_equal(rc, -1);	
}

int __wrap_open(const char *pathname, int flags, mode_t mode)
{
	return (int) mock();
}

int __wrap_rename(const char *oldpath, const char *newpath)
{
	return (int) mock();
}

static void test_correct_get_location(void **state)
{
	(void) state;

	int rc;

	FILE *fp = NULL;
	
	char *location_file = NULL;

	char expected[1 + 1] = ".";

	will_return(__wrap_getenv, expected);
	will_return(__wrap_access, 0);

	location_file = get_plugin_file_path(LOCATION_FILENAME, true);
	assert_non_null(location_file);

	assert_string_equal(location_file, "./location_info");
	
	fp = __real_fopen(location_file, "w+");

	assert_non_null(fp);
	
	assert_int_equal(fwrite("location_get", strlen("location_get") + 1, 1, fp), 1);
	assert_non_null(fp);

	rewind(fp);

	will_return(__wrap_getenv, ".");
	will_return(__wrap_access, 0);

	will_return(__wrap_fopen, fp);	

	char location[MAX_LOCATION_LENGTH];

	rc = get_location(location);
	
	assert_string_equal(location, "location_get");
	assert_int_equal(rc, 0);
}

static void test_incorrect_get_location(void **state)
{
	(void) state;
	char location[MAX_LOCATION_LENGTH];
	int rc;

	will_return(__wrap_getenv, NULL);	
	rc = get_location(location);
	assert_int_equal(rc, -1);
}

int __wrap_remove(const char *pathname)
{
	return (int) mock();
}

static void test_correct_set_location(void **state)
{
	(void) state;
	const char location[MAX_LOCATION_LENGTH] = "location_set";
	int rc;

	FILE *fp = NULL;
	
	char *location_file = NULL;

	char expected[1 + 1] = ".";

	will_return(__wrap_getenv, expected);
	will_return(__wrap_access, 0);

	location_file = get_plugin_file_path(LOCATION_FILENAME, true);
	assert_non_null(location_file);

	assert_string_equal(location_file, "./location_info");
	
	will_return(__wrap_getenv, "./");
	will_return(__wrap_access, 0);

	fp = __real_fopen(location_file, "w");
	assert_non_null(fp);
	
	will_return(__wrap_fopen, fp);		

	rc = set_location(location);
	assert_int_equal(rc, 0);
}

static void test_incorrect_set_location(void **state)
{
	(void) state;
	char location[MAX_LOCATION_LENGTH] = "loc";
	int rc;

	will_return(__wrap_getenv, NULL);	
	rc = set_location(location);
	assert_int_equal(rc, -1);
}
