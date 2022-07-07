#include "change.h"
#include "common.h"
#include "system/data/authentication/local_user.h"
#include "system/data/authentication/local_user/list.h"

#include <assert.h>
#include <sysrepo.h>
#include <sysrepo/xpath.h>

const char *system_authentication_change_user_extract_name(sr_session_ctx_t *session, const struct lyd_node *node);

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
	const char *user_name = NULL;
	system_local_user_element_t *found_user = NULL;
	system_local_user_t user = {0};

	assert(strcmp(node_name, "password") == 0);

	SRPLG_LOG_INF(PLUGIN_NAME, "Node Name: %s; Previous Value: %s, Value: %s; Operation: %d", node_name, change_ctx->previous_value, node_value, change_ctx->operation);

	// get username
	user_name = system_authentication_change_user_extract_name(session, change_ctx->node);
	if (!user_name) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_authentication_change_user_extract_name() failed");
		goto error_out;
	}

	// got the username - able to continue with operation
	switch (change_ctx->operation) {
		case SR_OP_CREATED:
			// find user and add password
			found_user = system_local_user_list_find(ctx->temp_users.created, user_name);
			if (!found_user) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_find() failed");
				goto error_out;
			}

			// user found - set password
			system_local_user_set_password(&found_user->user, node_value);

			break;
		case SR_OP_MODIFIED:
			// add user with the username and add the password to the modified list
			found_user = system_local_user_list_find(ctx->temp_users.modified, user_name);
			if (!found_user) {
				user.name = (char *) user_name;
				user.password = (char *) node_value;

				error = system_local_user_list_add(&ctx->temp_users.modified, user);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_add() error (%d)", error);
					goto error_out;
				}
			} else {
				error = system_local_user_set_password(&found_user->user, node_value);
				if (error) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_set_password() error (%d)", error);
					goto error_out;
				}
			}
			break;
		case SR_OP_DELETED:
			// if user not deleted remove his password - modified user
			found_user = system_local_user_list_find(ctx->temp_users.deleted, user_name);
			if (!found_user) {
				user.name = (char *) user_name;
				user.password = NULL;

				found_user = system_local_user_list_find(ctx->temp_users.modified, user_name);
				if (!found_user) {
					// create new modified user without a password
					error = system_local_user_list_add(&ctx->temp_users.modified, user);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_list_add() error (%d)", error);
						goto error_out;
					}
				} else {
					error = system_local_user_set_password(&found_user->user, NULL);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "system_local_user_set_password() error (%d)", error);
						goto error_out;
					}
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
	const char *node_name = LYD_NAME(change_ctx->node);
	const char *node_value = lyd_get_value(change_ctx->node);

	assert(strcmp(node_name, "authorized-key") == 0);

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

const char *system_authentication_change_user_extract_name(sr_session_ctx_t *session, const struct lyd_node *node)
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

	goto out;

error_out:
	name = NULL;

out:
	return name;
}