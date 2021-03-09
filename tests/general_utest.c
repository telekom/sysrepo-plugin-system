#include <stdarg.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>

#define PLUGIN
#include "general.c"

static void set_datetime_test(void **state);

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(set_datetime_test),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}

static void set_datetime_test(void **state)
{
	(void) state;

	return;
}
