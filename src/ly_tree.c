#include "ly_tree.h"
#include "common.h"

#include <linux/limits.h>
#include <sysrepo.h>

int system_ly_tree_create_container(const struct ly_ctx *ly_ctx, struct lyd_node *parent, struct lyd_node **store, const char *path)
{
	LY_ERR ly_error = LY_SUCCESS;

	ly_error = lyd_new_path(parent, ly_ctx, path, NULL, 0, store);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to create %s container libyang node", path);
		return -1;
	}

	return 0;
}

int system_ly_tree_create_list(const struct ly_ctx *ly_ctx, struct lyd_node *parent, struct lyd_node **store, const char *path, const char *key, const char *key_value)
{
	LY_ERR ly_error = LY_SUCCESS;
	char path_buffer[PATH_MAX] = {0};

	if (snprintf(path_buffer, sizeof(path_buffer), "%s[%s=%s]", path, key, key_value) < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf() failed");
		return -1;
	}

	ly_error = lyd_new_path(parent, ly_ctx, path_buffer, key_value, 0, store);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to create \"%s = %s\" list libyang node", path_buffer, key_value);
		return -1;
	}

	return 0;
}

int system_ly_tree_create_leaf(const struct ly_ctx *ly_ctx, struct lyd_node *parent, struct lyd_node **store, const char *path, const char *value)
{
	LY_ERR ly_error = LY_SUCCESS;

	ly_error = lyd_new_path(parent, ly_ctx, path, value, 0, store);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to create \"%s = %s\" leaf libyang node", path, value);
		return -1;
	}

	return 0;
}

int system_ly_tree_append_leaf_list(const struct ly_ctx *ly_ctx, struct lyd_node *parent, struct lyd_node **store, const char *path, const char *value)
{
	LY_ERR ly_error = LY_SUCCESS;

	ly_error = lyd_new_path(parent, ly_ctx, path, value, 0, store);
	if (ly_error != LY_SUCCESS) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unable to add value %s to the %s leaf list libyang node", value, path);
		return -1;
	}

	return 0;
}

int system_ly_tree_create_system_container(const struct ly_ctx *ly_ctx, struct lyd_node **system_container_node)
{
	return system_ly_tree_create_container(ly_ctx, NULL, system_container_node, SYSTEM_SYSTEM_CONTAINER_YANG_PATH);
}

int system_ly_tree_create_hostname(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, const char *hostname)
{
	return system_ly_tree_create_leaf(ly_ctx, system_container_node, NULL, "hostname", hostname);
}

int system_ly_tree_create_contact(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, const char *contact)
{
	return system_ly_tree_create_leaf(ly_ctx, system_container_node, NULL, "contact", contact);
}

int system_ly_tree_create_location(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, const char *location)
{
	return system_ly_tree_create_leaf(ly_ctx, system_container_node, NULL, "location", location);
}

int system_ly_tree_create_timezone_name(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, const char *timezone_name)
{
	return system_ly_tree_create_leaf(ly_ctx, system_container_node, NULL, "clock/timezone-name", timezone_name);
}

int system_ly_tree_create_system_state_container(const struct ly_ctx *ly_ctx, struct lyd_node **system_state_container_node)
{
	return system_ly_tree_create_container(ly_ctx, NULL, system_state_container_node, SYSTEM_STATE_YANG_PATH);
}