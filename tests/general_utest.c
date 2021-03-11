#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>

#define PLUGIN
#include "general.c"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static void test_correct_set_datetime(void **state);
static void test_incorrect_set_datetime(void **state);
static void test_correct_get_datetime_info(void **state);
static void test_incorrect_get_datetime_info(void **state);
static void test_correct_get_os_info(void **state);
static void test_incorrect_get_os_info(void **state);

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_correct_set_datetime),
		cmocka_unit_test(test_incorrect_set_datetime),
		cmocka_unit_test(test_correct_get_datetime_info),
		cmocka_unit_test(test_incorrect_get_datetime_info),
		cmocka_unit_test(test_correct_get_os_info),
		cmocka_unit_test(test_incorrect_get_os_info),
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
