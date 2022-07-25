#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <unistd.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "context.h"

// store API
#include "system/api/store.h"

// load API
#include "system/api/load.h"

// init functionality
static int setup(void **state);
static int teardown(void **state);

// tests
static void test_store_hostname_correct(void **state);
static void test_store_hostname_incorrect(void **state);
static void test_load_hostname_correct(void **state);
static void test_load_hostname_incorrect(void **state);

// wrapper functions
int __wrap_gethostname(char *buffer, size_t buffer_size);
int __wrap_sethostname(char *hostname, size_t len);

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_store_hostname_correct),
		cmocka_unit_test(test_store_hostname_incorrect),
		cmocka_unit_test(test_load_hostname_correct),
		cmocka_unit_test(test_load_hostname_incorrect),
	};

	return cmocka_run_group_tests(tests, setup, teardown);
}

static int setup(void **state)
{
	system_ctx_t *ctx = malloc(sizeof(system_ctx_t));
	if (!ctx) {
		return -1;
	}

	*ctx = (system_ctx_t){0};
	*state = ctx;

	return 0;
}

static int teardown(void **state)
{
	if (*state) {
		free(*state);
	}

	return 0;
}

static void test_store_hostname_correct(void **state)
{
	// TODO: see how to test because of AUGYANG usage
}

static void test_store_hostname_incorrect(void **state)
{
	system_ctx_t *ctx = *state;
	int rc = 0;

	will_return(__wrap_sethostname, -1);

	rc = system_store_hostname(ctx, "HOSTNAME");

	assert_int_equal(rc, -1);
}

static void test_load_hostname_correct(void **state)
{
	system_ctx_t *ctx = *state;
	char hostname_buffer[SYSTEM_HOSTNAME_LENGTH_MAX] = {0};
	int rc = 0;

	will_return(__wrap_gethostname, 0);

	rc = system_load_hostname(ctx, hostname_buffer);

	assert_int_equal(rc, 0);
	assert_string_equal(hostname_buffer, "HOSTNAME");
}

static void test_load_hostname_incorrect(void **state)
{
	system_ctx_t *ctx = *state;
	char hostname_buffer[SYSTEM_HOSTNAME_LENGTH_MAX] = {0};
	int rc = 0;

	will_return(__wrap_gethostname, -1);

	// assert the function returns an error
	rc = system_load_hostname(ctx, hostname_buffer);
	assert_int_equal(rc, -1);
}

int __wrap_gethostname(char *buffer, size_t buffer_size)
{
	int rc = 0;

	rc = snprintf(buffer, buffer_size, "HOSTNAME");
	if (rc < 0) {
		return rc;
	}

	return (int) mock();
}

int __wrap_sethostname(char *hostname, size_t len)
{
	return (int) mock();
}