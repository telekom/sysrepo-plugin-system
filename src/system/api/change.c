#include "change.h"
#include "load.h"
#include "store.h"
#include "sysrepo_types.h"

#include <unistd.h>

#include <sysrepo.h>
#include <srpc.h>

#include <assert.h>

static int system_change_hostname_create(system_ctx_t *ctx, const char *value);
static int system_change_hostname_modify(system_ctx_t *ctx, const char *old_value, const char *new_value);
static int system_change_hostname_delete(system_ctx_t *ctx);

static int system_change_contact_create(system_ctx_t *ctx, const char *value);
static int system_change_contact_modify(system_ctx_t *ctx, const char *old_value, const char *new_value);
static int system_change_contact_delete(system_ctx_t *ctx);

static int system_change_location_create(system_ctx_t *ctx, const char *value);
static int system_change_location_modify(system_ctx_t *ctx, const char *old_value, const char *new_value);
static int system_change_location_delete(system_ctx_t *ctx);

static int system_change_timezone_name_create(system_ctx_t *ctx, const char *value);
static int system_change_timezone_name_modify(system_ctx_t *ctx, const char *old_value, const char *new_value);
static int system_change_timezone_name_delete(system_ctx_t *ctx);

int system_change_contact(system_ctx_t *ctx, const srpc_change_node_t *change_node)
{
	int error = 0;
	return error;
}

int system_change_hostname(system_ctx_t *ctx, const srpc_change_node_t *change_node)
{
	int error = 0;

	// make sure we're getting the right node
	assert(strcmp(srpc_change_node_get_name(change_node), "hostname") == 0);

	// modify system value based on the operation on the tree node
	switch (srpc_change_node_get_operation(change_node)) {
		case SR_OP_CREATED:
			error = system_change_hostname_create(ctx, srpc_change_node_get_current_value(change_node));
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_change_hostname_create() error (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_MODIFIED:
			error = system_change_hostname_modify(ctx, srpc_change_node_get_previous_value(change_node), srpc_change_node_get_current_value(change_node));
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_change_hostname_modify() error (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_DELETED:
			// remove hostname
			error = system_change_hostname_delete(ctx);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_change_hostname_delete() error (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_MOVED:
			// N/A
			break;
	}

	goto out;

error_out:
	error = -1;

out:
	return error;
}

int system_change_location(system_ctx_t *ctx, const srpc_change_node_t *change_node)
{
	int error = 0;
	return error;
}

int system_change_timezone_name(system_ctx_t *ctx, const srpc_change_node_t *change_node)
{
	int error = 0;

	// make sure we're getting the right node
	assert(strcmp(srpc_change_node_get_name(change_node), "timezone-name") == 0);

	switch (srpc_change_node_get_operation(change_node)) {
		case SR_OP_CREATED:
			error = system_change_timezone_name_create(ctx, srpc_change_node_get_current_value(change_node));
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_change_timezone_name_create() failed (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_MODIFIED:
			error = system_change_timezone_name_modify(ctx, srpc_change_node_get_previous_value(change_node), srpc_change_node_get_current_value(change_node));
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_change_timezone_name_modify() failed (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_DELETED:
			error = system_change_timezone_name_delete(ctx);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_change_timezone_name_delete() failed (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_MOVED:
			break;
	}

	goto out;

error_out:
	error = -1;

out:
	return error;
}

static int system_change_hostname_create(system_ctx_t *ctx, const char *value)
{
	int error = 0;

	error = system_store_hostname(ctx, value);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_store_hostname() error (%d)", error);
		return -1;
	}

	return 0;
}

static int system_change_hostname_modify(system_ctx_t *ctx, const char *old_value, const char *new_value)
{
	(void) old_value;
	return system_change_hostname_create(ctx, new_value);
}

static int system_change_hostname_delete(system_ctx_t *ctx)
{
	int error = 0;

	error = system_store_hostname(ctx, "none");
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_store_hostname() error (%d)", error);
		return -1;
	}

	return 0;
}

static int system_change_contact_create(system_ctx_t *ctx, const char *value)
{
	int error = 0;

	return error;
}

static int system_change_contact_modify(system_ctx_t *ctx, const char *old_value, const char *new_value)
{
	int error = 0;

	return error;
}

static int system_change_contact_delete(system_ctx_t *ctx)
{
	int error = 0;

	return error;
}

static int system_change_location_create(system_ctx_t *ctx, const char *value)
{
	int error = 0;

	return error;
}

static int system_change_location_modify(system_ctx_t *ctx, const char *old_value, const char *new_value)
{
	int error = 0;

	return error;
}

static int system_change_location_delete(system_ctx_t *ctx)
{
	int error = 0;

	return error;
}

static int system_change_timezone_name_create(system_ctx_t *ctx, const char *value)
{
	int error = 0;

	error = system_store_timezone_name(ctx, value);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_store_timezone_name() error (%d)", error);
		return -1;
	}

	return 0;
}

static int system_change_timezone_name_modify(system_ctx_t *ctx, const char *old_value, const char *new_value)
{
	(void) old_value;
	return system_change_timezone_name_create(ctx, new_value);
}

static int system_change_timezone_name_delete(system_ctx_t *ctx)
{
	(void) ctx;

	int error = 0;

	error = access(SYSTEM_LOCALTIME_FILE, F_OK);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "/etc/localtime doesn't exist");
		goto error_out;
	}

	error = unlink(SYSTEM_LOCALTIME_FILE);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "unlink() failed (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:
	return error;
}