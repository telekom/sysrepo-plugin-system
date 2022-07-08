#include "change.h"
#include "common.h"
#include "libyang/tree_data.h"
#include "system/api/authentication/store.h"
#include "system/data/authentication/local_user.h"
#include "system/data/authentication/local_user/list.h"
#include "types.h"
#include "umgmt/db.h"
#include "umgmt/types.h"
#include "umgmt/user.h"

#include <assert.h>
#include <linux/limits.h>
#include <sysrepo.h>
#include <sysrepo/xpath.h>

#include <unistd.h>
#include <utlist.h>

static int system_authentication_change_user_extract_name(sr_session_ctx_t *session, const struct lyd_node *node, char *name_buffer, size_t buffer_size);
static int delete_home_directory(const char *username);
static int system_authentication_authorized_key_change_name(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx);
static int system_authentication_authorized_key_change_algorithm(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx);
static int system_authentication_authorized_key_change_key_data(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx);

int system_authentication_user_apply_changes(system_ctx_t *ctx)
{
	int error = 0;
	um_db_t *user_db = NULL;
	um_user_t *temp_user = NULL;

	system_local_user_element_t *user_iter = NULL;
	system_authorized_key_element_t *key_iter = NULL;

	SRPLG_LOG_INF(PLUGIN_NAME, "Created users:");
	LL_FOREACH(ctx->temp_users.created, user_iter)
	{
		SRPLG_LOG_INF(PLUGIN_NAME, "\t %s : %s", user_iter->user.name, user_iter->user.password);
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Modified users:");
	LL_FOREACH(ctx->temp_users.modified, user_iter)
	{
		SRPLG_LOG_INF(PLUGIN_NAME, "\t %s : %s", user_iter->user.name, user_iter->user.password);
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Deleted users:");
	LL_FOREACH(ctx->temp_users.deleted, user_iter)
	{
		SRPLG_LOG_INF(PLUGIN_NAME, "\t %s", user_iter->user.name);
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Created user keys:");
	LL_FOREACH(ctx->temp_users.keys.created, user_iter)
	{
		SRPLG_LOG_INF(PLUGIN_NAME, "\tKeys for user %s:", user_iter->user.name);
		LL_FOREACH(user_iter->user.key_head, key_iter)
		{
			SRPLG_LOG_INF(PLUGIN_NAME, "\t\t %s : %s : %s", key_iter->key.name, key_iter->key.algorithm, key_iter->key.data);
		}
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Modified user keys:");
	LL_FOREACH(ctx->temp_users.keys.modified, user_iter)
	{
		SRPLG_LOG_INF(PLUGIN_NAME, "\tKeys for user %s:", user_iter->user.name);
		LL_FOREACH(user_iter->user.key_head, key_iter)
		{
			SRPLG_LOG_INF(PLUGIN_NAME, "\t\t %s : %s : %s", key_iter->key.name, key_iter->key.algorithm, key_iter->key.data);
		}
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Deleted user keys:");
	LL_FOREACH(ctx->temp_users.keys.deleted, user_iter)
	{
		SRPLG_LOG_INF(PLUGIN_NAME, "\tKeys for user %s:", user_iter->user.name);
		LL_FOREACH(user_iter->user.key_head, key_iter)
		{
			SRPLG_LOG_INF(PLUGIN_NAME, "\t\t %s : %s : %s", key_iter->key.name, key_iter->key.algorithm, key_iter->key.data);
		}
	}

	// #define APPLY_CHANGES

#ifdef APPLY_CHANGES

	// for created users - use store API
	error = system_authentication_store_user(ctx, ctx->temp_users.created);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_store_user() error (%d)", error);
		goto error_out;
	}

	// for modified users - iterate and change passwords
	user_db = um_db_new();

	error = um_db_load(user_db);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "um_db_load() error (%d)", error);
		goto error_out;
	}

	LL_FOREACH(ctx->temp_users.modified, iter)
	{
		// get user
		temp_user = um_db_get_user(user_db, iter->user.name);
		if (!temp_user) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to find user %s in the user database", iter->user.name);
			goto error_out;
		}

		// change user password hash
		error = um_user_set_password_hash(temp_user, iter->user.password);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "um_user_set_password_hash() error (%d)", error);
			goto error_out;
		}
	}

	// for deleted users - delete recursively home directory and remove user from the database
	LL_FOREACH(ctx->temp_users.deleted, iter)
	{
		// 1. remove home directory of the user
		error = delete_home_directory(iter->user.name);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "delete_home_directory() error (%d)", error);
			goto error_out;
		}

		// 2. remove user and user group from the database
		error = um_db_delete_user(user_db, iter->user.name);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "um_db_delete_user() error (%d) for user %s", error, iter->user.name);
			goto error_out;
		}
		error = um_db_delete_group(user_db, iter->user.name);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "um_db_delete_group() error (%d) for user %s", error, iter->user.name);
			goto error_out;
		}
	}

	// after user changes handle authentication changes

#endif

	goto out;

error_out:
	error = -1;

out:
	return error;
}

int system_authentication_change_user_name(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	system_ctx_t *ctx = priv;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);
	system_local_user_t user = {0};

	assert(strcmp(node_name, "name") == 0);

	SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	system_local_user_init(&user);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			// create new user and add it to the created list
			user.name = (char *) node_value;

			error = system_local_user_list_add(&ctx->temp_users.created, user);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_add() error (%d)", error);
				goto error_out;
			}

			break;
		case SR_OP_MODIFIED:
			// can't modify name
			break;
		case SR_OP_DELETED:
			user.name = (char *) node_value;

			error = system_local_user_list_add(&ctx->temp_users.deleted, user);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_add() error (%d)", error);
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

int system_authentication_change_user_password(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	system_ctx_t *ctx = priv;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);
	char username_buffer[33] = {0};
	system_local_user_element_t *found_user = NULL;

	assert(strcmp(node_name, "password") == 0);

	SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	// get username
	error = system_authentication_change_user_extract_name(session, change_ctx->node, username_buffer, sizeof(username_buffer));
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_change_user_extract_name() error (%d)", error);
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Recieved user name: %s", username_buffer);

	// got the username - able to continue with operation
	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			// find user and add password
			found_user = system_local_user_list_find(ctx->temp_users.created, username_buffer);
			if (!found_user) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_find() failed");
				goto error_out;
			}

			// user found - set password
			system_local_user_set_password(&found_user->user, node_value);

			break;
		case SR_OP_MODIFIED:
			// add user with the username and add the password to the modified list
			found_user = system_local_user_list_find(ctx->temp_users.modified, username_buffer);
			if (found_user) {
				error = system_local_user_set_password(&found_user->user, node_value);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_set_password() error (%d)", error);
					goto error_out;
				}
			} else {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_find() failed for user %s", username_buffer);
				goto error_out;
			}
			break;
		case SR_OP_DELETED:
			// if user not deleted remove his password - modified user
			found_user = system_local_user_list_find(ctx->temp_users.deleted, username_buffer);
			if (!found_user) {
				found_user = system_local_user_list_find(ctx->temp_users.modified, username_buffer);
				if (found_user) {
					error = system_local_user_set_password(&found_user->user, NULL);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_set_password() error (%d)", error);
						goto error_out;
					}
				} else {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_find() failed for user %s", username_buffer);
					goto error_out;
				}
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

int system_authentication_change_user_authorized_key(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	system_ctx_t *ctx = priv;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);
	char xpath_buffer[PATH_MAX] = {0};
	char path_buffer[PATH_MAX] = {0};

	const char *node_path = lyd_path(change_ctx->node, LYD_PATH_STD, path_buffer, sizeof(path_buffer));

	assert(strcmp(node_name, "authorized-key") == 0);

	SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);
	SRPLG_LOG_INF(PLUGIN_NAME, "Node Path: %s", node_path);

	// name change
	error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/name", node_path);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
		goto error_out;
	}
	error = srpc_iterate_changes(ctx, session, xpath_buffer, system_authentication_authorized_key_change_name);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for name failed: %d", error);
		goto error_out;
	}

	// algorithm change
	error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/algorithm", node_path);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
		goto error_out;
	}
	error = srpc_iterate_changes(ctx, session, xpath_buffer, system_authentication_authorized_key_change_algorithm);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for algorithm failed: %d", error);
		goto error_out;
	}

	// key-data change
	error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/key-data", node_path);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error: %d", error);
		goto error_out;
	}
	error = srpc_iterate_changes(ctx, session, xpath_buffer, system_authentication_authorized_key_change_key_data);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "srpc_iterate_changes() for key-data failed: %d", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:

	return error;
}

static int system_authentication_change_user_extract_name(sr_session_ctx_t *session, const struct lyd_node *node, char *name_buffer, size_t buffer_size)
{
	int error = 0;

	const char *name = NULL;

	sr_xpath_ctx_t xpath_ctx = {0};
	char path_buffer[PATH_MAX] = {0};

	// get node full path
	error = (lyd_path(node, LYD_PATH_STD, path_buffer, sizeof(path_buffer)) == NULL);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "lyd_path() failed");
		goto error_out;
	}

	// extract key
	name = sr_xpath_key_value(path_buffer, "user", "name", &xpath_ctx);
	if (!name) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_xpath_key_value() failed");
		goto error_out;
	}

	// store to buffer
	error = snprintf(name_buffer, buffer_size, "%s", name);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed");
		goto error_out;
	}

	error = 0;
	goto out;

error_out:
	error = -1;

out:
	return error;
}

static int delete_home_directory(const char *username)
{
	int error = 0;
	char home_buffer[PATH_MAX] = {0};
	char command_buffer[PATH_MAX + 100] = {0};

	error = snprintf(home_buffer, sizeof(home_buffer), "/home/%s", username);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error (%d)", error);
		goto error_out;
	}

	error = snprintf(command_buffer, sizeof(command_buffer), "rm -r %s", home_buffer);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error (%d)", error);
		goto error_out;
	}

	// rm -r should return 0
	error = system(command_buffer);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system() failed for command \"%s\"", command_buffer);
		goto error_out;
	}

	error = 0;
	goto out;

error_out:
	error = -1;

out:

	return error;
}

static int system_authentication_authorized_key_change_name(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	system_ctx_t *ctx = priv;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);

	assert(strcmp(node_name, "name") == 0);

	SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			break;
		case SR_OP_MODIFIED:
			break;
		case SR_OP_DELETED:
			break;
		case SR_OP_MOVED:
			break;
	}

	return error;
}

static int system_authentication_authorized_key_change_algorithm(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	system_ctx_t *ctx = priv;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);

	assert(strcmp(node_name, "algorithm") == 0);

	SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			break;
		case SR_OP_MODIFIED:
			break;
		case SR_OP_DELETED:
			break;
		case SR_OP_MOVED:
			break;
	}

	return error;
}

static int system_authentication_authorized_key_change_key_data(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	system_ctx_t *ctx = priv;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);

	assert(strcmp(node_name, "key-data") == 0);

	SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			break;
		case SR_OP_MODIFIED:
			break;
		case SR_OP_DELETED:
			break;
		case SR_OP_MOVED:
			break;
	}

	return error;
}
