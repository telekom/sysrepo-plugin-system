#include "ly_tree.h"
#include "common.h"

#include <linux/limits.h>
#include <sysrepo.h>

#include <srpc/ly_tree.h>

int system_ly_tree_create_system_container(const struct ly_ctx *ly_ctx, struct lyd_node **system_container_node)
{
	return srpc_ly_tree_create_container(ly_ctx, NULL, system_container_node, SYSTEM_SYSTEM_CONTAINER_YANG_PATH);
}

int system_ly_tree_create_clock_container(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, struct lyd_node **clock_container_node)
{
	return srpc_ly_tree_create_container(ly_ctx, system_container_node, clock_container_node, "clock");
}

int system_ly_tree_create_ntp_container(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, struct lyd_node **ntp_container_node)
{
	return srpc_ly_tree_create_container(ly_ctx, system_container_node, ntp_container_node, "ntp");
}

int system_ly_tree_create_dns_resolver_container(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, struct lyd_node **dns_resolver_container_node)
{
	return srpc_ly_tree_create_container(ly_ctx, system_container_node, dns_resolver_container_node, "dns-resolver");
}

int system_ly_tree_create_authentication_container(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, struct lyd_node **authentication_container_node)
{
	return srpc_ly_tree_create_container(ly_ctx, system_container_node, authentication_container_node, "authentication");
}

int system_ly_tree_create_hostname(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, const char *hostname)
{
	return srpc_ly_tree_create_leaf(ly_ctx, system_container_node, NULL, "hostname", hostname);
}

int system_ly_tree_create_contact(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, const char *contact)
{
	return srpc_ly_tree_create_leaf(ly_ctx, system_container_node, NULL, "contact", contact);
}

int system_ly_tree_create_location(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, const char *location)
{
	return srpc_ly_tree_create_leaf(ly_ctx, system_container_node, NULL, "location", location);
}

int system_ly_tree_create_timezone_name(const struct ly_ctx *ly_ctx, struct lyd_node *clock_container_node, const char *timezone_name)
{
	return srpc_ly_tree_create_leaf(ly_ctx, clock_container_node, NULL, "timezone-name", timezone_name);
}

int system_ly_tree_create_system_state_container(const struct ly_ctx *ly_ctx, struct lyd_node **system_state_container_node)
{
	return srpc_ly_tree_create_container(ly_ctx, NULL, system_state_container_node, SYSTEM_STATE_YANG_PATH);
}