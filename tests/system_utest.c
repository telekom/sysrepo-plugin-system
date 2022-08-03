// cmocka headers
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

// stdlib
#include <unistd.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// sd-bus
#include <systemd/sd-bus.h>

// plugin code
#include "context.h"

// store API
#include "system/api/store.h"

// load API
#include "system/api/load.h"

// check API
#include "system/api/check.h"

// ntp load API
#include "system/api/dns_resolver/load.h"

// init functionality
static int setup(void **state);
static int teardown(void **state);

// tests
// store
static void test_store_hostname_correct(void **state);
static void test_store_hostname_incorrect(void **state);
static void test_store_timezone_name_correct(void **state);
static void test_store_timezone_name_incorrect(void **state);

// load
static void test_load_hostname_correct(void **state);
static void test_load_hostname_incorrect(void **state);
static void test_load_timezone_name_correct(void **state);

// check
static void test_check_hostname_correct(void **state);
static void test_check_hostname_incorrect(void **state);
static void test_check_timezone_name_correct(void **state);
static void test_check_timezone_name_incorrect(void **state);

// ntp load
static void test_load_dns_resolver_search_correct(void **state);
static void test_load_dns_resolver_server_correct(void **state);

// wrapper functions
int __wrap_gethostname(char *buffer, size_t buffer_size);
int __wrap_sethostname(char *hostname, size_t len);
int __wrap_unlink(const char *pathname);
int __wrap_symlink(const char *target, const char *linkpath);
int __wrap_sr_apply_changes(sr_session_ctx_t *session, uint32_t timeout_ms);

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_store_hostname_correct),
		cmocka_unit_test(test_store_hostname_incorrect),
		cmocka_unit_test(test_store_timezone_name_correct),
		cmocka_unit_test(test_store_timezone_name_incorrect),
		cmocka_unit_test(test_load_hostname_correct),
		cmocka_unit_test(test_load_hostname_incorrect),
		cmocka_unit_test(test_load_timezone_name_correct),
		cmocka_unit_test(test_check_hostname_correct),
		cmocka_unit_test(test_check_hostname_incorrect),
		cmocka_unit_test(test_check_timezone_name_correct),
		cmocka_unit_test(test_check_timezone_name_incorrect),
		cmocka_unit_test(test_load_dns_resolver_search_correct),
		cmocka_unit_test(test_load_dns_resolver_server_correct),
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
	system_ctx_t *ctx = *state;
	int rc = 0;

#ifdef AUGYANG
	will_return(__wrap_sr_apply_changes, 0);
#endif

	will_return(__wrap_sethostname, 0);

	rc = system_store_hostname(ctx, "HOSTNAME");

	assert_int_equal(rc, 0);
}

static void test_store_hostname_incorrect(void **state)
{
	system_ctx_t *ctx = *state;
	int rc = 0;

	will_return(__wrap_sethostname, -1);

	rc = system_store_hostname(ctx, "HOSTNAME");

	assert_int_equal(rc, -1);
}

static void test_store_timezone_name_correct(void **state)
{
	system_ctx_t *ctx = *state;
	int rc = 0;

	will_return(__wrap_unlink, 0);
	will_return(__wrap_symlink, 0);

	rc = system_store_timezone_name(ctx, "Europe/Ljubljana");
	assert_int_equal(rc, 0);
}

static void test_store_timezone_name_incorrect(void **state)
{
	system_ctx_t *ctx = *state;
	int rc = 0;

	rc = system_store_timezone_name(ctx, "Foo/Bar");
	assert_int_not_equal(rc, 0);
}

static void test_load_hostname_correct(void **state)
{
	system_ctx_t *ctx = *state;
	char hostname_buffer[SYSTEM_HOSTNAME_LENGTH_MAX] = {0};
	int rc = 0;

	expect_memory(__wrap_gethostname, buffer, hostname_buffer, SYSTEM_HOSTNAME_LENGTH_MAX);
	expect_value(__wrap_gethostname, buffer_size, SYSTEM_HOSTNAME_LENGTH_MAX);

	will_return(__wrap_gethostname, "HOSTNAME");
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

	expect_memory(__wrap_gethostname, buffer, hostname_buffer, SYSTEM_HOSTNAME_LENGTH_MAX);
	expect_value(__wrap_gethostname, buffer_size, SYSTEM_HOSTNAME_LENGTH_MAX);

	will_return(__wrap_gethostname, "\0");
	will_return(__wrap_gethostname, -1);

	// assert the function returns an error
	rc = system_load_hostname(ctx, hostname_buffer);
	assert_int_equal(rc, -1);
	assert_string_equal(hostname_buffer, "\0");
}

static void test_load_timezone_name_correct(void **state)
{
	system_ctx_t *ctx = *state;
	char timezone_name_buffer[SYSTEM_TIMEZONE_NAME_LENGTH_MAX] = {0};
	int rc = 0;

	rc = system_load_timezone_name(ctx, timezone_name_buffer);

	assert_int_equal(rc, 0);
	assert_string_not_equal(timezone_name_buffer, "\0");
}

static void test_check_hostname_correct(void **state)
{
	system_ctx_t *ctx = *state;
	char hostname_buffer[SYSTEM_HOSTNAME_LENGTH_MAX] = {0};
	srpc_check_status_t status = srpc_check_status_none;

	expect_memory(__wrap_gethostname, buffer, hostname_buffer, SYSTEM_HOSTNAME_LENGTH_MAX);
	expect_value(__wrap_gethostname, buffer_size, SYSTEM_HOSTNAME_LENGTH_MAX);

	will_return(__wrap_gethostname, "HOSTNAME");
	will_return(__wrap_gethostname, 0);

	status = system_check_hostname(ctx, "HOSTNAME");

	assert_int_equal(status, srpc_check_status_equal);
}

static void test_check_hostname_incorrect(void **state)
{
	system_ctx_t *ctx = *state;
	char hostname_buffer[SYSTEM_HOSTNAME_LENGTH_MAX] = {0};
	srpc_check_status_t status = srpc_check_status_none;

	expect_memory(__wrap_gethostname, buffer, hostname_buffer, SYSTEM_HOSTNAME_LENGTH_MAX);
	expect_value(__wrap_gethostname, buffer_size, SYSTEM_HOSTNAME_LENGTH_MAX);

	will_return(__wrap_gethostname, "HOSTNAME");
	will_return(__wrap_gethostname, 0);

	status = system_check_hostname(ctx, "FOO");

	assert_int_equal(status, srpc_check_status_non_existant);
}

static void test_check_timezone_name_correct(void **state)
{
	system_ctx_t *ctx = *state;
	char timezone_name_buffer[SYSTEM_TIMEZONE_NAME_LENGTH_MAX] = {0};
	int rc = 0;
	srpc_check_status_t status = srpc_check_status_none;


	rc = system_load_timezone_name(ctx, timezone_name_buffer);

	assert_int_equal(rc, 0);

	status = system_check_timezone_name(ctx, timezone_name_buffer);

	assert_int_equal(status, srpc_check_status_equal);
}

static void test_check_timezone_name_incorrect(void **state)
{
	system_ctx_t *ctx = *state;
	srpc_check_status_t status = srpc_check_status_none;

	status = system_check_timezone_name(ctx, "FOO/BAR");

	assert_int_equal(status, srpc_check_status_non_existant);
}

static void test_load_dns_resolver_search_correct(void **state)
{
	system_ctx_t *ctx = *state;
	system_dns_search_element_t *head = NULL;
	int rc = 0;

	head = calloc(sizeof(system_dns_search_element_t), 
		      sizeof(system_dns_search_element_t));

	rc = system_dns_resolver_load_search(ctx, &head);

	assert_int_equal(rc, 0);
}

static void test_load_dns_resolver_server_correct(void **state)
{
	system_ctx_t *ctx = *state;
	system_dns_server_element_t *head = NULL;
	int rc = 0;

	head = calloc(sizeof(system_dns_server_element_t), 
		      sizeof(system_dns_server_element_t));

	rc = system_dns_resolver_load_server(ctx, &head);

	assert_int_equal(rc, 0);
}

int __wrap_gethostname(char *buffer, size_t buffer_size)
{
	check_expected_ptr(buffer);
	check_expected(buffer_size);

	int rc = 0;
	char *hostname = mock_ptr_type(char *);

	rc = snprintf(buffer, buffer_size, "%s", hostname);
	if (rc < 0) {
		return rc;
	}

	return (int) mock();
}

int __wrap_sethostname(char *hostname, size_t len)
{
	return (int) mock();
}

int __wrap_unlink(const char *pathname)
{
	return (int) mock();
}

int __wrap_symlink(const char *target, const char *linkpath)
{
	return (int) mock();
}

int __wrap_sr_apply_changes(sr_session_ctx_t *session, uint32_t timeout_ms)
{
	return (int) mock();
}

