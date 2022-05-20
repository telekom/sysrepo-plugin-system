#ifndef SYSTEM_PLUGIN_LY_TREE_H
#define SYSTEM_PLUGIN_LY_TREE_H

#include <libyang/libyang.h>

// running/startup

// base helpers
int system_ly_tree_create_container(const struct ly_ctx *ly_ctx, struct lyd_node *parent, struct lyd_node **store, const char *path);
int system_ly_tree_create_list(const struct ly_ctx *ly_ctx, struct lyd_node *parent, struct lyd_node **store, const char *path, const char *key, const char *key_value);
int system_ly_tree_create_leaf(const struct ly_ctx *ly_ctx, struct lyd_node *parent, struct lyd_node **store, const char *path, const char *value);
int system_ly_tree_append_leaf_list(const struct ly_ctx *ly_ctx, struct lyd_node *parent, struct lyd_node **store, const char *path, const char *value);

// actual values
int system_ly_tree_create_system_container(const struct ly_ctx *ly_ctx, struct lyd_node **system_container_node);
int system_ly_tree_create_hostname(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, const char *hostname);
int system_ly_tree_create_contact(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, const char *contact);
int system_ly_tree_create_location(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, const char *location);
int system_ly_tree_create_timezone_name(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, const char *timezone_name);

// operational
int system_ly_tree_create_system_state_container(const struct ly_ctx *ly_ctx, struct lyd_node **system_state_container_node);

#endif // SYSTEM_PLUGIN_LY_TREE_H