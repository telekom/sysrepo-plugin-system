#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>

#define PLUGIN
#include "general.c"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static void test_correct_set_datetime(void **state);
static void test_incorrect_set_datetime(void **state);

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_correct_set_datetime),
		cmocka_unit_test(test_incorrect_set_datetime),
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
