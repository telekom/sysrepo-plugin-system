#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

static void simple_utest(void **state);

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(simple_utest),
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}

static void simple_utest(void **state)
{
	(void) state;
}