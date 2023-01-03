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
#include "core/ly_tree.h"
#include "core/common.h"

#include <linux/limits.h>
#include <sysrepo.h>

#include <srpc/ly_tree.h>

int system_ly_tree_create_system(const struct ly_ctx *ly_ctx, struct lyd_node **system_container_node)
{
	return srpc_ly_tree_create_container(ly_ctx, NULL, system_container_node, SYSTEM_SYSTEM_CONTAINER_YANG_PATH);
}

int system_ly_tree_create_clock(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, struct lyd_node **clock_container_node)
{
	return srpc_ly_tree_create_container(ly_ctx, system_container_node, clock_container_node, "clock");
}

int system_ly_tree_create_ntp(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, struct lyd_node **ntp_container_node)
{
	return srpc_ly_tree_create_container(ly_ctx, system_container_node, ntp_container_node, "ntp");
}

int system_ly_tree_create_dns_resolver(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, struct lyd_node **dns_resolver_container_node)
{
	return srpc_ly_tree_create_container(ly_ctx, system_container_node, dns_resolver_container_node, "dns-resolver");
}

int system_ly_tree_create_authentication(const struct ly_ctx *ly_ctx, struct lyd_node *system_container_node, struct lyd_node **authentication_container_node)
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

int system_ly_tree_create_ntp_enabled(const struct ly_ctx *ly_ctx, struct lyd_node *ntp_container_node, const char *enabled)
{
	return srpc_ly_tree_create_leaf(ly_ctx, ntp_container_node, NULL, "enabled", enabled);
}

int system_ly_tree_create_ntp_server(const struct ly_ctx *ly_ctx, struct lyd_node *ntp_container_node, struct lyd_node **server_list_node, const char *name)
{
	return srpc_ly_tree_create_list(ly_ctx, ntp_container_node, server_list_node, "server", "name", name);
}

int system_ly_tree_create_ntp_server_address(const struct ly_ctx *ly_ctx, struct lyd_node *server_list_node, const char *address)
{
	return srpc_ly_tree_create_leaf(ly_ctx, server_list_node, NULL, "udp/address", address);
}

int system_ly_tree_create_ntp_server_port(const struct ly_ctx *ly_ctx, struct lyd_node *server_list_node, const char *port)
{
	return srpc_ly_tree_create_leaf(ly_ctx, server_list_node, NULL, "udp/port", port);
}

int system_ly_tree_create_ntp_server_association_type(const struct ly_ctx *ly_ctx, struct lyd_node *server_list_node, const char *association_type)
{
	return srpc_ly_tree_create_leaf(ly_ctx, server_list_node, NULL, "association-type", association_type);
}

int system_ly_tree_create_ntp_server_iburst(const struct ly_ctx *ly_ctx, struct lyd_node *server_list_node, const char *iburst)
{
	return srpc_ly_tree_create_leaf(ly_ctx, server_list_node, NULL, "iburst", iburst);
}

int system_ly_tree_create_ntp_server_prefer(const struct ly_ctx *ly_ctx, struct lyd_node *server_list_node, const char *prefer)
{
	return srpc_ly_tree_create_leaf(ly_ctx, server_list_node, NULL, "prefer", prefer);
}

int system_ly_tree_append_dns_resolver_search(const struct ly_ctx *ly_ctx, struct lyd_node *dns_resolver_container_node, const char *value)
{
	return srpc_ly_tree_append_leaf_list(ly_ctx, dns_resolver_container_node, NULL, "search", value);
}

int system_ly_tree_create_dns_resolver_server(const struct ly_ctx *ly_ctx, struct lyd_node *dns_resolver_container_node, struct lyd_node **server_list_node, const char *name)
{
	return srpc_ly_tree_create_list(ly_ctx, dns_resolver_container_node, server_list_node, "server", "name", name);
}

int system_ly_tree_create_dns_resolver_server_address(const struct ly_ctx *ly_ctx, struct lyd_node *server_list_node, const char *address)
{
	return srpc_ly_tree_create_leaf(ly_ctx, server_list_node, NULL, "udp-and-tcp/address", address);
}

int system_ly_tree_create_dns_resolver_server_port(const struct ly_ctx *ly_ctx, struct lyd_node *server_list_node, const char *port)
{
	return srpc_ly_tree_create_leaf(ly_ctx, server_list_node, NULL, "udp-and-tcp/port", port);
}

int system_ly_tree_create_authentication_user(const struct ly_ctx *ly_ctx, struct lyd_node *authentication_container_node, struct lyd_node **user_list_node, const char *name)
{
	return srpc_ly_tree_create_list(ly_ctx, authentication_container_node, user_list_node, "user", "name", name);
}

int system_ly_tree_create_authentication_user_password(const struct ly_ctx *ly_ctx, struct lyd_node *user_list_node, const char *password)
{
	return srpc_ly_tree_create_leaf(ly_ctx, user_list_node, NULL, "password", password);
}

int system_ly_tree_create_authentication_user_authorized_key(const struct ly_ctx *ly_ctx, struct lyd_node *user_list_node, struct lyd_node **authorized_key_list_node, const char *name)
{
	return srpc_ly_tree_create_list(ly_ctx, user_list_node, authorized_key_list_node, "authorized-key", "name", name);
}

int system_ly_tree_create_authentication_user_authorized_key_algorithm(const struct ly_ctx *ly_ctx, struct lyd_node *authorized_key_list_node, const char *algorithm)
{
	return srpc_ly_tree_create_leaf(ly_ctx, authorized_key_list_node, NULL, "algorithm", algorithm);
}

int system_ly_tree_create_authentication_user_authorized_key_data(const struct ly_ctx *ly_ctx, struct lyd_node *authorized_key_list_node, const char *data)
{
	return srpc_ly_tree_create_leaf(ly_ctx, authorized_key_list_node, NULL, "key-data", data);
}

int system_ly_tree_create_system_state(const struct ly_ctx *ly_ctx, struct lyd_node *parent_node, struct lyd_node **system_state_container_node)
{
	return srpc_ly_tree_create_container(ly_ctx, parent_node, system_state_container_node, SYSTEM_STATE_YANG_PATH);
}

int system_ly_tree_create_state_platform(const struct ly_ctx *ly_ctx, struct lyd_node *system_state_container_node, struct lyd_node **platform_container_node)
{
	return srpc_ly_tree_create_container(ly_ctx, system_state_container_node, platform_container_node, "platform");
}

int system_ly_tree_create_state_platform_os_name(const struct ly_ctx *ly_ctx, struct lyd_node *platform_container_node, const char *os_name)
{
	return srpc_ly_tree_create_leaf(ly_ctx, platform_container_node, NULL, "os-name", os_name);
}

int system_ly_tree_create_state_platform_os_release(const struct ly_ctx *ly_ctx, struct lyd_node *platform_container_node, const char *os_release)
{
	return srpc_ly_tree_create_leaf(ly_ctx, platform_container_node, NULL, "os-release", os_release);
}

int system_ly_tree_create_state_platform_os_version(const struct ly_ctx *ly_ctx, struct lyd_node *platform_container_node, const char *os_version)
{
	return srpc_ly_tree_create_leaf(ly_ctx, platform_container_node, NULL, "os-version", os_version);
}

int system_ly_tree_create_state_platform_machine(const struct ly_ctx *ly_ctx, struct lyd_node *platform_container_node, const char *machine)
{
	return srpc_ly_tree_create_leaf(ly_ctx, platform_container_node, NULL, "machine", machine);
}

int system_ly_tree_create_state_clock(const struct ly_ctx *ly_ctx, struct lyd_node *system_state_container_node, struct lyd_node **clock_container_node)
{
	return srpc_ly_tree_create_container(ly_ctx, system_state_container_node, clock_container_node, "clock");
}

int system_ly_tree_create_state_clock_current_datetime(const struct ly_ctx *ly_ctx, struct lyd_node *clock_container_node, const char *current_datetime)
{
	return srpc_ly_tree_create_leaf(ly_ctx, clock_container_node, NULL, "current-datetime", current_datetime);
}

int system_ly_tree_create_state_clock_boot_datetime(const struct ly_ctx *ly_ctx, struct lyd_node *clock_container_node, const char *boot_datetime)
{
	return srpc_ly_tree_create_leaf(ly_ctx, clock_container_node, NULL, "boot-datetime", boot_datetime);
}