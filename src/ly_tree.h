#ifndef SYSTEM_PLUGIN_LY_TREE_H
#define SYSTEM_PLUGIN_LY_TREE_H

#include <libyang/libyang.h>

// containers
int system_ly_tree_create_system(const struct ly_ctx *ly_ctx, struct lyd_node **system_container_node);
int system_ly_tree_create_clock(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, struct lyd_node **clock_container_node);
int system_ly_tree_create_ntp(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, struct lyd_node **ntp_container_node);
int system_ly_tree_create_dns_resolver(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, struct lyd_node **dns_resolver_container_node);
int system_ly_tree_create_authentication(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, struct lyd_node **authentication_container_node);

// system leafs
int system_ly_tree_create_hostname(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, const char *hostname);
int system_ly_tree_create_contact(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, const char *contact);
int system_ly_tree_create_location(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, const char *location);
int system_ly_tree_create_timezone_name(const struct ly_ctx *ly_ctx, struct lyd_node *clock_container_node, const char *timezone_name);

// ntp
int system_ly_tree_create_ntp_enabled(const struct ly_ctx *ly_ctx, struct lyd_node *ntp_container_node, const char *enabled);
int system_ly_tree_create_ntp_server(const struct ly_ctx *ly_ctx, struct lyd_node *ntp_container_node, struct lyd_node **server_list_node, const char *name);
int system_ly_tree_create_ntp_server_address(const struct ly_ctx *ly_ctx, struct lyd_node *server_list_node, const char *address);
int system_ly_tree_create_ntp_server_port(const struct ly_ctx *ly_ctx, struct lyd_node *server_list_node, const char *port);
int system_ly_tree_create_ntp_server_association_type(const struct ly_ctx *ly_ctx, struct lyd_node *server_list_node, const char *association_type);
int system_ly_tree_create_ntp_server_iburst(const struct ly_ctx *ly_ctx, struct lyd_node *server_list_node, const char *iburst);
int system_ly_tree_create_ntp_server_prefer(const struct ly_ctx *ly_ctx, struct lyd_node *server_list_node, const char *prefer);

// dns-resolver
int system_ly_tree_append_dns_resolver_search(const struct ly_ctx *ly_ctx, struct lyd_node *dns_resolver_container_node, const char *value);
int system_ly_tree_create_dns_resolver_server(const struct ly_ctx *ly_ctx, struct lyd_node *dns_resolver_container_node, struct lyd_node **server_list_node, const char *name);
int system_ly_tree_create_dns_resolver_server_address(const struct ly_ctx *ly_ctx, struct lyd_node *server_list_node, const char *address);
int system_ly_tree_create_dns_resolver_server_port(const struct ly_ctx *ly_ctx, struct lyd_node *server_list_node, const char *port);

// operational
int system_ly_tree_create_system_state(const struct ly_ctx *ly_ctx, struct lyd_node *parent_node, struct lyd_node **system_state_container_node);

// platform
int system_ly_tree_create_state_platform(const struct ly_ctx *ly_ctx, struct lyd_node *system_state_container_node, struct lyd_node **platform_container_node);
int system_ly_tree_create_state_platform_os_name(const struct ly_ctx *ly_ctx, struct lyd_node *platform_container_node, const char *os_name);
int system_ly_tree_create_state_platform_os_release(const struct ly_ctx *ly_ctx, struct lyd_node *platform_container_node, const char *os_release);
int system_ly_tree_create_state_platform_os_version(const struct ly_ctx *ly_ctx, struct lyd_node *platform_container_node, const char *os_version);
int system_ly_tree_create_state_platform_machine(const struct ly_ctx *ly_ctx, struct lyd_node *platform_container_node, const char *machine);

// clock
int system_ly_tree_create_state_clock(const struct ly_ctx *ly_ctx, struct lyd_node *system_state_container_node, struct lyd_node **clock_container_node);
int system_ly_tree_create_state_clock_current_datetime(const struct ly_ctx *ly_ctx, struct lyd_node *clock_container_node, const char *current_datetime);
int system_ly_tree_create_state_clock_boot_datetime(const struct ly_ctx *ly_ctx, struct lyd_node *clock_container_node, const char *boot_datetime);

#endif // SYSTEM_PLUGIN_LY_TREE_H