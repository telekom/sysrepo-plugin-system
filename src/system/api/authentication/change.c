/*
 * telekom / sysrepo-plugin-system
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2022 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "change.h"
#include "common.h"
#include "libyang/tree_data.h"
#include "system/api/authentication/store.h"
#include "system/data/authentication/authorized_key.h"
#include "system/data/authentication/authorized_key/list.h"
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
static int system_authentication_change_user_authorized_key_extract_name(sr_session_ctx_t *session, const struct lyd_node *node, char *name_buffer, size_t buffer_size);
static int delete_home_directory(const char *username);

int system_authentication_user_apply_changes(system_ctx_t *ctx)
{
	int error = 0;
	um_db_t *user_db = NULL;
	um_user_t *temp_user = NULL;
	bool has_user_changes = false;
	char file_path_buffer[PATH_MAX] = {0};

	system_local_user_element_t *user_iter = NULL;
	system_authorized_key_element_t *key_iter = NULL;

	SRPLG_LOG_INF(PLUGIN_NAME, "Created users:");
	LL_FOREACH(ctx->temp_users.created, user_iter)
	{
		SRPLG_LOG_INF(PLUGIN_NAME, "\t %s : %s", user_iter->user.name, user_iter->user.password);
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "(Non)modified users:");
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

#define APPLY_CHANGES

#ifdef APPLY_CHANGES

	// for created users - use store API
	error = system_authentication_store_user(ctx, ctx->temp_users.created);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_store_user() error (%d)", error);
		goto error_out;
	}

	user_db = um_db_new();

	error = um_db_load(user_db);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "um_db_load() error (%d)", error);
		goto error_out;
	}

	if (ctx->temp_users.modified || ctx->temp_users.deleted) {
		has_user_changes = true;
	}

	// for modified users - iterate and change passwords
	LL_FOREACH(ctx->temp_users.modified, user_iter)
	{
		// get user
		temp_user = um_db_get_user(user_db, user_iter->user.name);
		if (!temp_user) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to find user %s in the user database", user_iter->user.name);
			goto error_out;
		}

		// if the password has changed - store new value
		if ((user_iter->user.password == NULL && um_user_get_password_hash(temp_user) != NULL) || strcmp(user_iter->user.password, um_user_get_password_hash(temp_user))) {
			SRPLG_LOG_INF(PLUGIN_NAME, "Password changed for %s: %s --> %s", user_iter->user.name, um_user_get_password_hash(temp_user), user_iter->user.password);
			error = um_user_set_password_hash(temp_user, user_iter->user.password);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "um_user_set_password_hash() error (%d)", error);
				goto error_out;
			}
		}
	}

	// for deleted users - delete recursively home directory and remove user from the database
	LL_FOREACH(ctx->temp_users.deleted, user_iter)
	{
		// 1. remove home directory of the user
		error = delete_home_directory(user_iter->user.name);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "delete_home_directory() error (%d)", error);
			goto error_out;
		}

		// 2. remove user and user group from the database
		error = um_db_delete_user(user_db, user_iter->user.name);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "um_db_delete_user() error (%d) for user %s", error, user_iter->user.name);
			goto error_out;
		}
		error = um_db_delete_group(user_db, user_iter->user.name);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "um_db_delete_group() error (%d) for user %s", error, user_iter->user.name);
			goto error_out;
		}
	}

	if (has_user_changes) {
		error = um_db_store(user_db);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "um_db_store() error (%d)", error);
			goto error_out;
		}
	}

	LL_FOREACH(ctx->temp_users.keys.created, user_iter)
	{
		error = system_authentication_store_user_authorized_key(ctx, user_iter->user.name, user_iter->user.key_head);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_store_user_authorized_key() error (%d) for user %s", error, user_iter->user.name);
			goto error_out;
		}
	}

	LL_FOREACH(ctx->temp_users.keys.modified, user_iter)
	{
		error = system_authentication_store_user_authorized_key(ctx, user_iter->user.name, user_iter->user.key_head);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_store_user_authorized_key() error (%d) for user %s", error, user_iter->user.name);
			goto error_out;
		}
	}

	LL_FOREACH(ctx->temp_users.keys.deleted, user_iter)
	{
		LL_FOREACH(user_iter->user.key_head, key_iter)
		{
			if (snprintf(file_path_buffer, sizeof(file_path_buffer), "/home/%s/.ssh/%s", user_iter->user.name, key_iter->key.name) < 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() error");
				goto error_out;
			}

			// file path written - remove file
			error = remove(file_path_buffer);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "remove() failed (%d)", error);
				goto error_out;
			}
		}
	}
#else
	goto error_out;
#endif

	goto out;

error_out:
	error = -1;

out:
	if (user_db) {
		um_db_free(user_db);
	}

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
				// user was not created but now the password is - modified user
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
			} else {
				// user found - set password
				error = system_local_user_set_password(&found_user->user, node_value);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_set_password() error (%d)", error);
					goto error_out;
				}
			}

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
			found_user = system_local_user_list_find(ctx->temp_users.deleted, username_buffer);
			if (!found_user) {
				// if user not deleted remove his password - modified user
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

static int system_authentication_change_user_authorized_key_extract_name(sr_session_ctx_t *session, const struct lyd_node *node, char *name_buffer, size_t buffer_size)
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
	name = sr_xpath_key_value(path_buffer, "authorized-key", "name", &xpath_ctx);
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

int system_authentication_user_change_authorized_key_name(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	system_ctx_t *ctx = priv;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);
	char username_buffer[33] = {0};
	system_local_user_element_t *user_el = NULL;
	system_local_user_t temp_user = {0};
	system_authorized_key_t temp_key = {0};

	assert(strcmp(node_name, "name") == 0);

	SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	// get username
	error = system_authentication_change_user_extract_name(session, change_ctx->node, username_buffer, sizeof(username_buffer));
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_change_user_extract_name() error (%d)", error);
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Recieved user name: %s", username_buffer);

	// setup data
	temp_user.name = username_buffer;
	temp_key.name = (char *) node_value;

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			// check for user in the list
			user_el = system_local_user_list_find(ctx->temp_users.keys.created, username_buffer);
			if (!user_el) {
				// user does not exist - create one

				// add user to the list
				error = system_local_user_list_add(&ctx->temp_users.keys.created, temp_user);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_add() error (%d)", error);
					goto error_out;
				}

				// get user element
				user_el = system_local_user_list_find(ctx->temp_users.keys.created, username_buffer);
				if (!user_el) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_find() failed");
					goto error_out;
				}
			}

			// add new key to the user keys list
			error = system_authorized_key_list_add(&user_el->user.key_head, temp_key);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_list_add() error (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_MODIFIED:
			// check for user in the list
			user_el = system_local_user_list_find(ctx->temp_users.keys.modified, username_buffer);
			if (!user_el) {
				// user does not exist - create one

				// add user to the list
				error = system_local_user_list_add(&ctx->temp_users.keys.modified, temp_user);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_add() error (%d)", error);
					goto error_out;
				}

				// get user element
				user_el = system_local_user_list_find(ctx->temp_users.keys.modified, username_buffer);
				if (!user_el) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_find() failed");
					goto error_out;
				}
			}

			// add new key to the user keys list
			error = system_authorized_key_list_add(&user_el->user.key_head, temp_key);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_list_add() error (%d)", error);
				goto error_out;
			}
			break;
		case SR_OP_DELETED:
			// check for user in the list
			user_el = system_local_user_list_find(ctx->temp_users.keys.deleted, username_buffer);
			if (!user_el) {
				// user does not exist - create one

				// add user to the list
				error = system_local_user_list_add(&ctx->temp_users.keys.deleted, temp_user);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_add() error (%d)", error);
					goto error_out;
				}

				// get user element
				user_el = system_local_user_list_find(ctx->temp_users.keys.deleted, username_buffer);
				if (!user_el) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_find() failed");
					goto error_out;
				}
			}

			// add new key to the user keys list
			error = system_authorized_key_list_add(&user_el->user.key_head, temp_key);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_list_add() error (%d)", error);
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

int system_authentication_user_change_authorized_key_algorithm(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	system_ctx_t *ctx = priv;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);

	char username_buffer[33] = {0};
	char key_buffer[PATH_MAX] = {0};

	system_local_user_element_t *user_el = NULL;
	system_authorized_key_element_t *key_el = NULL;
	system_local_user_element_t **users_list = NULL;

	assert(strcmp(node_name, "algorithm") == 0);

	SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	// get username
	error = system_authentication_change_user_extract_name(session, change_ctx->node, username_buffer, sizeof(username_buffer));
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_change_user_extract_name() error (%d)", error);
		goto error_out;
	}

	// get key name
	error = system_authentication_change_user_authorized_key_extract_name(session, change_ctx->node, key_buffer, sizeof(key_buffer));
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_change_user_authorized_key_extract_name() error (%d)", error);
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Recieved user name: %s", username_buffer);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			users_list = &ctx->temp_users.keys.created;
			break;
		case SR_OP_MODIFIED:
			users_list = &ctx->temp_users.keys.modified;
			break;
		case SR_OP_DELETED:
			// if deleted, information not needed
			goto out;
			break;
		case SR_OP_MOVED:
			break;
	}

	user_el = system_local_user_list_find(*users_list, username_buffer);
	if (!user_el) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_find() failed");
		goto error_out;
	}

	key_el = system_authorized_key_list_find(user_el->user.key_head, key_buffer);
	if (!key_el) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_list_find() failed");
		goto error_out;
	}

	// set algorithm
	error = system_authorized_key_set_algorithm(&key_el->key, node_value);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_set_algorithm() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:

	return error;
}

int system_authentication_user_change_authorized_key_key_data(void *priv, sr_session_ctx_t *session, const srpc_change_ctx_t *change_ctx)
{
	int error = 0;
	system_ctx_t *ctx = priv;
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);

	char username_buffer[33] = {0};
	char key_buffer[PATH_MAX] = {0};

	system_local_user_element_t *user_el = NULL;
	system_authorized_key_element_t *key_el = NULL;
	system_local_user_element_t **users_list = NULL;

	assert(strcmp(node_name, "key-data") == 0);

	SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	// get username
	error = system_authentication_change_user_extract_name(session, change_ctx->node, username_buffer, sizeof(username_buffer));
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_change_user_extract_name() error (%d)", error);
		goto error_out;
	}

	// get key name
	error = system_authentication_change_user_authorized_key_extract_name(session, change_ctx->node, key_buffer, sizeof(key_buffer));
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_change_user_authorized_key_extract_name() error (%d)", error);
		goto error_out;
	}

	SRPLG_LOG_INF(PLUGIN_NAME, "Recieved user name: %s", username_buffer);

	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			users_list = &ctx->temp_users.keys.created;
			break;
		case SR_OP_MODIFIED:
			users_list = &ctx->temp_users.keys.modified;
			break;
		case SR_OP_DELETED:
			// if deleted, information not needed
			goto out;
			break;
		case SR_OP_MOVED:
			break;
	}

	user_el = system_local_user_list_find(*users_list, username_buffer);
	if (!user_el) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_find() failed");
		goto error_out;
	}

	key_el = system_authorized_key_list_find(user_el->user.key_head, key_buffer);
	if (!key_el) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_list_find() failed");
		goto error_out;
	}

	// set key-data
	error = system_authorized_key_set_data(&key_el->key, node_value);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_authorized_key_set_data() error (%d)", error);
		goto error_out;
	}

	goto out;

error_out:
	error = -1;

out:

	return error;
}