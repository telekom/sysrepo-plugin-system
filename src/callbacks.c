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

// project headers
#include "callbacks.h"
#include "common.h"
#include "utils/memory.h"
#include "context.h"
#include "utils/ntp/server_list.h"
#include "utils/dns/server.h"
#include "utils/dns/search.h"
#include "utils/user_auth/user_authentication.h"
#ifdef SYSTEMD
#else
#include "utils/dns/resolv_conf.h"
#endif

// libs
#include <sysrepo.h>
#include <sysrepo/xpath.h>

// std
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/sendfile.h>
#include <sys/utsname.h>
#include <sys/reboot.h>
#include <sys/dir.h>
#include <fcntl.h>
#define __USE_XOPEN
#define _GNU_SOURCE
#include <time.h>

int system_module_change_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data)
{
	int error = 0;
	system_ctx_t *ctx = (system_ctx_t *) private_data;
	sr_session_ctx_t *startup_session = ctx->startup_session;
	sr_change_iter_t *system_change_iter = NULL;
	sr_change_oper_t operation = SR_OP_CREATED;
	const struct lyd_node *node = NULL;
	char path[512] = {0};
	const char *prev_value = NULL;
	const char *prev_list = NULL;
	int prev_default = false;
	char *node_xpath = NULL;
	char *node_value = NULL;
	bool ntp_change = false;
	bool dns_servers_change = false;
	bool user_change = false;

	SRPLG_LOG_INF(PLUGIN_NAME, "module_name: %s, xpath: %s, event: %d, request_id: %" PRIu32, module_name, xpath, event, request_id);

	if (event == SR_EV_ABORT) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "aborting changes for: %s", xpath);
		error = -1;
		goto error_out;
	}

	if (event == SR_EV_DONE) {
		error = sr_copy_config(startup_session, BASE_YANG_MODEL, SR_DS_RUNNING, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_copy_config error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	if (event == SR_EV_CHANGE) {
		sprintf(path, "%s//.", xpath);

		error = sr_get_changes_iter(session, path, &system_change_iter);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_get_changes_iter error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		while (sr_get_change_tree_next(session, system_change_iter, &operation, &node, &prev_value, &prev_list, &prev_default) == SR_ERR_OK) {
			node_xpath = lyd_path(node, LYD_PATH_STD, NULL, 0);

			if (node->schema->nodetype == LYS_LEAF || node->schema->nodetype == LYS_LEAFLIST) {
				node_value = xstrdup(lyd_get_value(node));
			}

			SRPLG_LOG_DBG(PLUGIN_NAME, "node_xpath: %s; prev_val: %s; node_val: %s; operation: %d", node_xpath, prev_value, node_value, operation);

			if (node->schema->nodetype == LYS_LEAF || node->schema->nodetype == LYS_LEAFLIST) {
				if (operation == SR_OP_CREATED || operation == SR_OP_MODIFIED) {
					error = system_set_config_value(ctx, node_xpath, node_value, operation);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "set_config_value error (%d)", error);
						goto error_out;
					}

					if (strncmp(node_xpath, NTP_YANG_PATH, sizeof(NTP_YANG_PATH) - 1) == 0) {
						ntp_change = true;
					}

					if (strncmp(node_xpath, DNS_RESOLVER_SERVER_YANG_PATH, sizeof(DNS_RESOLVER_SERVER_YANG_PATH) - 1) == 0) {
						dns_servers_change = true;
					}

					if (strncmp(node_xpath, AUTHENTICATION_USER_YANG_PATH, strlen(AUTHENTICATION_USER_YANG_PATH)) == 0) {
						user_change = true;
					}
				} else if (operation == SR_OP_DELETED) {
					error = system_set_config_value(ctx, node_xpath, node_value, operation);
					if (error) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "set_config_value error (%d)", error);
						goto error_out;
					}

					if (strncmp(node_xpath, NTP_YANG_PATH, strlen(NTP_YANG_PATH)) == 0) {
						ntp_change = true;
					}

					if (strncmp(node_xpath, DNS_RESOLVER_SERVER_YANG_PATH, strlen(DNS_RESOLVER_SERVER_YANG_PATH)) == 0) {
						dns_servers_change = true;
					}

					if (strncmp(node_xpath, AUTHENTICATION_USER_YANG_PATH, strlen(AUTHENTICATION_USER_YANG_PATH)) == 0) {
						user_change = true;
					}
				}
			}
			FREE_SAFE(node_xpath);
			FREE_SAFE(node_value);
		}

		if (ntp_change) {
			// save data to ntp.conf
			error = save_ntp_config(&ctx->ntp_servers);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "save_ntp_config error (%d)", error);
				goto error_out;
			}
		}

		if (dns_servers_change == true) {
			SRPLG_LOG_DBG(PLUGIN_NAME, "Dumping DNS servers configuration...");
			error = dns_server_list_dump(&ctx->dns_servers_head);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "dns_server_list_dump_config (%d)", error);
				goto error_out;
			}
		}

		if (user_change) {
			// save users to system
			error = local_user_array_set_new_users(&ctx->local_users);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "set_new_users error (%d)", error);
				goto error_out;
			}
		}
	}
	goto out;

error_out:
	error = SR_ERR_CALLBACK_FAILED;

out:
	FREE_SAFE(node_xpath);
	FREE_SAFE(node_value);
	sr_free_change_iter(system_change_iter);

	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

int system_state_data_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data)
{
	int error = SR_ERR_OK;
	char *os_name = NULL;
	char *os_release = NULL;
	char *os_version = NULL;
	char *machine = NULL;
	char current_datetime[DATETIME_BUF_SIZE] = {0};
	char boot_datetime[DATETIME_BUF_SIZE] = {0};

	error = system_get_os_info(&os_name, &os_release, &os_version, &machine);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "get_os_info error: %s", strerror(errno));
		goto out;
	}

	error = system_get_datetime_info(current_datetime, boot_datetime);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "get_datetime_info error: %s", strerror(errno));
		goto out;
	}

	// TODO: replace this with the above call to store_values_to_datastore
	const struct ly_ctx *ly_ctx = NULL;
	if (*parent == NULL) {
		ly_ctx = sr_acquire_context(sr_session_get_connection(session));
		if (ly_ctx == NULL) {
			goto out;
		}
		lyd_new_path(*parent, ly_ctx, SYSTEM_STATE_YANG_MODEL, NULL, 0, 0);
	}

	lyd_new_path(*parent, NULL, OS_NAME_YANG_PATH, os_name, 0, 0);
	lyd_new_path(*parent, NULL, OS_RELEASE_YANG_PATH, os_release, 0, 0);
	lyd_new_path(*parent, NULL, OS_VERSION_YANG_PATH, os_version, 0, 0);
	lyd_new_path(*parent, NULL, OS_MACHINE_YANG_PATH, machine, 0, 0);

	lyd_new_path(*parent, NULL, CURR_DATETIME_YANG_PATH, current_datetime, 0, 0);
	lyd_new_path(*parent, NULL, BOOT_DATETIME_YANG_PATH, boot_datetime, 0, 0);

	//values = NULL;

out:
	if (os_name != NULL) {
		FREE_SAFE(os_name);
	}
	if (os_release != NULL) {
		FREE_SAFE(os_release);
	}
	if (os_version != NULL) {
		FREE_SAFE(os_version);
	}
	if (machine != NULL) {
		FREE_SAFE(machine);
	}

	return error ? SR_ERR_CALLBACK_FAILED : SR_ERR_OK;
}

int system_rpc_cb(sr_session_ctx_t *session, uint32_t subscription_id, const char *op_path, const sr_val_t *input, const size_t input_cnt, sr_event_t event, uint32_t request_id, sr_val_t **output, size_t *output_cnt, void *private_data)
{
	int error = SR_ERR_OK;
	char *datetime = NULL;

	if (strcmp(op_path, SET_CURR_DATETIME_YANG_PATH) == 0) {
		if (input_cnt != 1) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "system_rpc_cb: input_cnt != 1");
			goto error_out;
		}

		datetime = input[0].data.string_val;

		error = system_set_datetime(datetime);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "set_datetime error: %s", strerror(errno));
			goto error_out;
		}

		error = sr_set_item_str(session, CURR_DATETIME_YANG_PATH, datetime, NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		error = sr_apply_changes(session, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_apply_changes error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}

		SRPLG_LOG_INF(PLUGIN_NAME, "system_rpc_cb: CURR_DATETIME_YANG_PATH and system time successfully set!");

	} else if (strcmp(op_path, RESTART_YANG_PATH) == 0) {
		sync();
		system("shutdown -r");
		SRPLG_LOG_INF(PLUGIN_NAME, "system_rpc_cb: restarting the system!");
	} else if (strcmp(op_path, SHUTDOWN_YANG_PATH) == 0) {
		sync();
		system("shutdown -P");
		SRPLG_LOG_INF(PLUGIN_NAME, "system_rpc_cb: shutting down the system!");
	} else {
		SRPLG_LOG_ERR(PLUGIN_NAME, "system_rpc_cb: invalid path %s", op_path);
		goto error_out;
	}

	return SR_ERR_OK;

error_out:
	return SR_ERR_CALLBACK_FAILED;
}

int system_set_config_value(system_ctx_t *ctx, const char *xpath, const char *value, sr_change_oper_t operation)
{
	int error = 0;
	bool prepared_changes = false;

	if (strcmp(xpath, HOSTNAME_YANG_PATH) == 0) {
		if (operation == SR_OP_DELETED) {
			error = sethostname("none", strnlen("none", HOST_NAME_MAX));
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "sethostname error: %s", strerror(errno));
			}

#ifdef AUGYANG
			SRPLG_LOG_DBG(PLUGIN_NAME, "removing /etc/hostname value - setting to \"none\"");
			error = sr_set_item_str(ctx->startup_session, "/hostname:hostname[config-file=\'/etc/hostname\']/hostname", "none", NULL, 0);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error: %s", sr_strerror(error));
			} else {
				SRPLG_LOG_DBG(PLUGIN_NAME, "/etc/hostname deleted");
				prepared_changes = true;
			}
#endif
		} else {
			error = sethostname(value, strnlen(value, HOST_NAME_MAX));
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "sethostname error: %s", strerror(errno));
			}

#ifdef AUGYANG
			SRPLG_LOG_DBG(PLUGIN_NAME, "setting /etc/hostname value using srds_augeas DS");
			error = sr_set_item_str(ctx->startup_session, "/hostname:hostname[config-file=\'/etc/hostname\']/hostname", value, NULL, 0);
			if (error) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error: %s", sr_strerror(error));
			} else {
				SRPLG_LOG_DBG(PLUGIN_NAME, "/etc/hostname set");
				prepared_changes = true;
			}
#endif
		}
	} else if (strcmp(xpath, CONTACT_YANG_PATH) == 0) {
		if (operation == SR_OP_DELETED) {
			error = system_set_contact_info("");
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "set_contact_info error: %s", strerror(errno));
			}
		} else {
			error = system_set_contact_info(value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "set_contact_info error: %s", strerror(errno));
			}
		}
	} else if (strcmp(xpath, LOCATION_YANG_PATH) == 0) {
		if (operation == SR_OP_DELETED) {
			error = system_set_location("none");
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "setlocation error: %s", strerror(errno));
			}
		} else {
			error = system_set_location(value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "setlocation error: %s", strerror(errno));
			}
		}
	} else if (strcmp(xpath, TIMEZONE_NAME_YANG_PATH) == 0) {
		if (operation == SR_OP_DELETED) {
			// check if the /etc/localtime symlink exists
			error = access(LOCALTIME_FILE, F_OK);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "/etc/localtime doesn't exist; unlink/delete timezone error: %s", strerror(errno));
			}

			error = unlink(LOCALTIME_FILE);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "unlinking/deleting timezone error: %s", strerror(errno));
			}
		} else {
			error = system_set_timezone(value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "set_timezone error: %s", strerror(errno));
			}
		}
	} else if (strcmp(xpath, TIMEZONE_OFFSET_YANG_PATH) == 0) {
		// timezone-utc-offset leaf
		// https://linux.die.net/man/5/tzfile
		// https://linux.die.net/man/8/zic
	} else if (strncmp(xpath, NTP_YANG_PATH, strlen(NTP_YANG_PATH)) == 0) {
		if (operation == SR_OP_DELETED) {
			error = system_set_ntp(ctx, xpath, "");
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "set_ntp error: %s", strerror(errno));
			}
		} else {
			error = system_set_ntp(ctx, xpath, (char *) value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "set_ntp error: %s", strerror(errno));
			}
		}
	} else if (strncmp(xpath, DNS_RESOLVER_YANG_PATH, sizeof(DNS_RESOLVER_YANG_PATH) - 1) == 0) {
		error = system_set_dns(ctx, xpath, (char *) value, operation);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "set_dns error");
		}
	} else if (strncmp(xpath, AUTHENTICATION_USER_YANG_PATH, strlen(AUTHENTICATION_USER_YANG_PATH)) == 0) {
		if (operation == SR_OP_DELETED) {
			error = system_set_user_authentication(ctx, xpath, "");
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "set_authentication_user error");
			}
		} else {
			error = system_set_user_authentication(ctx, xpath, (char *) value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "set_authentication_user error");
			}
		}
	}

#ifdef AUGYANG
	if (prepared_changes) {
		SRPLG_LOG_DBG(PLUGIN_NAME, "applying /etc/hostname changes");
		error = sr_apply_changes(ctx->startup_session, 0);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_apply_changes error: %s", sr_strerror(error));
		}
	}
#endif

	return error;
}

int system_set_ntp(system_ctx_t *ctx, const char *xpath, char *value)
{
	int error = 0;

	if (strcmp(xpath, NTP_ENABLED_YANG_PATH) == 0) {
		if (strcmp(value, "true") == 0) {
			// TODO: replace "system()" call with sd-bus later if needed
			error = system("systemctl enable --now ntpd");
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "\"systemctl enable --now ntpd\" failed with return value: %d", error);
				/* Debian based systems have a service file named
				 * ntp.service instead of ntpd.service for some reason...
				 */
				SRPLG_LOG_ERR(PLUGIN_NAME, "trying systemctl enable --now ntp instead");
				error = system("systemctl enable --now ntp");
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "\"systemctl enable --now ntp\" failed with return value: %d", error);
					return -1;
				}
			}
			// TODO: check if ntpd was enabled
		} else if (strcmp(value, "false") == 0) {
			// TODO: add - 'systemctl stop ntpd' as well ?
			error = system("systemctl stop ntpd");
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "\"systemctl stop ntpd\" failed with return value: %d", error);
				/* Debian based systems have a service file named
				 * ntp.service instead of ntpd.service for some reason...
				 */
				SRPLG_LOG_ERR(PLUGIN_NAME, "trying systemctl stop ntp instead");
				error = system("systemctl stop ntp");
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "\"systemctl stop ntp\" failed with return value: %d", error);
					return -1;
				} else {
					error = system("systemctl disable ntp");
					if (error != 0) {
						SRPLG_LOG_ERR(PLUGIN_NAME, "\"systemctl disable ntpd\" failed with return value: %d", error);
						return -1;
					}
				}
			} else {
				error = system("systemctl disable ntpd");
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "\"systemctl disable ntpd\" failed with return value: %d", error);
					return -1;
				}
			}
			// TODO: check if ntpd was disabled
		}
	} else if (strncmp(xpath, NTP_SERVER_YANG_PATH, strlen(NTP_SERVER_YANG_PATH)) == 0) {
		char *ntp_node = NULL;
		char *ntp_server_name = NULL;
		sr_xpath_ctx_t state = {0};

		ntp_node = sr_xpath_node_name((char *) xpath);
		ntp_server_name = sr_xpath_key_value((char *) xpath, "server", "name", &state);

		if (strcmp(ntp_node, "name") == 0) {
			if (strcmp(value, "") == 0) {
				error = ntp_server_array_set_delete(&ctx->ntp_servers, ntp_server_name, true);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "ntp_server_list_set_delete error");
					return -1;
				}
			} else {
				error = ntp_server_array_add_server(&ctx->ntp_servers, value);
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "error adding new ntp server");
					return -1;
				}
			}

		} else if (strcmp(ntp_node, "address") == 0) {
			error = ntp_server_array_set_address(&ctx->ntp_servers, ntp_server_name, value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "error setting ntp server address");
				return -1;
			}
			// set the address and name to a file
			error = system_ntp_set_server_name(value, ntp_server_name);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "ntp_set_server_name error");
				return -1;
			}

		} else if (strcmp(ntp_node, "port") == 0) {
			error = ntp_server_array_set_port(&ctx->ntp_servers, ntp_server_name, value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "error setting ntp server port");
				return -1;
			}

		} else if (strcmp(ntp_node, "association-type") == 0) {
			error = ntp_server_array_set_assoc_type(&ctx->ntp_servers, ntp_server_name, value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "error setting ntp server association-type");
				return -1;
			}

		} else if (strcmp(ntp_node, "iburst") == 0) {
			if (strcmp(value, "true") == 0) {
				error = ntp_server_array_set_iburst(&ctx->ntp_servers, ntp_server_name, "iburst");
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "error setting ntp server iburst");
					return -1;
				}
			} else {
				error = ntp_server_array_set_iburst(&ctx->ntp_servers, ntp_server_name, "");
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "error setting ntp server iburst");
					return -1;
				}
			}

		} else if (strcmp(ntp_node, "prefer") == 0) {
			if (strcmp(value, "true") == 0) {
				error = ntp_server_array_set_prefer(&ctx->ntp_servers, ntp_server_name, "prefer");
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "error setting ntp server prefer");
					return -1;
				}
			} else {
				error = ntp_server_array_set_prefer(&ctx->ntp_servers, ntp_server_name, "");
				if (error != 0) {
					SRPLG_LOG_ERR(PLUGIN_NAME, "error setting ntp server prefer");
					return -1;
				}
			}
		}
	}

	return 0;
}

int system_set_dns(system_ctx_t *ctx, const char *xpath, char *value, sr_change_oper_t operation)
{
	int err = 0;
	char *nn = sr_xpath_node_name(xpath);

	SRPLG_LOG_DBG(PLUGIN_NAME, "Xpath for dns-resolver: %s -> %s = %s", xpath, nn, value);

	// first check for server -> if not server change then watch for other leafs/leaf lists
	if (strncmp(xpath, DNS_RESOLVER_SERVER_YANG_PATH, sizeof(DNS_RESOLVER_SERVER_YANG_PATH) - 1) == 0) {
		char *name = NULL;
		sr_xpath_ctx_t state = {0};

		name = sr_xpath_key_value((char *) xpath, "server", "name", &state);

		if (strcmp(nn, "name") == 0) {
			if (operation == SR_OP_CREATED) {
				SRPLG_LOG_DBG(PLUGIN_NAME, "Creating server '%s'", value);
				err = dns_server_list_add(&ctx->dns_servers_head, value);
			} else if (operation == SR_OP_DELETED) {
				SRPLG_LOG_DBG(PLUGIN_NAME, "deleting server '%s'", value);
				err = dns_server_list_delete(&ctx->dns_servers_head, value);
			}
		} else if (strcmp(nn, "address") == 0) {
			// set server name
			SRPLG_LOG_DBG(PLUGIN_NAME, "Setting server %s address to '%s'", name, value);
			err = dns_server_list_set_address(&ctx->dns_servers_head, name, value);
		} else if (strcmp(nn, "port") == 0) {
			// set server port
			err = dns_server_list_set_port(&ctx->dns_servers_head, name, atoi(value));
		}
	} else if (strcmp(nn, "search") == 0) {
		switch (operation) {
			case SR_OP_CREATED:
				SRPLG_LOG_DBG(PLUGIN_NAME, "Adding dns-resolver 'search' = '%s'", value);
				err = dns_search_add(value);
				break;
			case SR_OP_MODIFIED:
				break;
			case SR_OP_DELETED:
				SRPLG_LOG_DBG(PLUGIN_NAME, "Removing dns-resolver 'search' = '%s'", value);
				err = dns_search_remove(value);
				break;
			case SR_OP_MOVED:
				break;
		}
	} else if (strcmp(nn, "timeout") == 0) {
#ifdef SYSTEMD
		// unknown for systemd
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unsupported option 'timeout'... Aborting...");
		err = -1;
#else
		SRPLG_LOG_DBG(PLUGIN_NAME, "Setting DNS timeout value...");
		err = set_dns_timeout(value);
#endif
	} else if (strcmp(nn, "attempts") == 0) {
#ifdef SYSTEMD
		// unknown for systemd
		SRPLG_LOG_ERR(PLUGIN_NAME, "Unsupported option 'attempts'... Aborting...");
		err = -1;
#else
		SRPLG_LOG_DBG(PLUGIN_NAME, "Setting DNS attempts value...");
		err = set_dns_attempts(value);
#endif
	}
	return err;
}

int system_set_user_authentication(system_ctx_t *ctx, const char *xpath, char *value)
{
	int error = 0;
	char *user_node = NULL;
	char *user_name = NULL;
	char *user_ssh_file_name = NULL;
	char *tmp_ssh_file = NULL;
	sr_xpath_ctx_t state = {0};
	char *tmp_xpath = NULL;

	tmp_xpath = xstrdup(xpath);

	user_node = sr_xpath_node_name((char *) xpath);
	user_name = sr_xpath_key_value((char *) xpath, "user", "name", &state);
	user_ssh_file_name = sr_xpath_key_value((char *) tmp_xpath, "authorized-key", "name", &state);

	if (user_ssh_file_name == NULL) {
		if (strcmp(user_node, "name") == 0) {
			// don't set empty string as a new user
			// only happens when you delete a user
			if (strcmp(value, "") == 0) {
				FREE_SAFE(tmp_xpath);
				return 0;
			}
			error = local_user_array_add_user(&ctx->local_users, value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "local_user_add_user error");
				return -1;
			}
		} else if (strcmp(user_node, "password") == 0) {
			error = local_user_array_set_password(&ctx->local_users, user_name, value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "local_user_set_password error");
				return -1;
			}
		}
	} else {
		// check if ssh public key file has .pub extension
		if (!has_pub_extension(user_ssh_file_name)) {
			// if it doesn't, add it
			size_t ssh_file_len = 0;

			ssh_file_len = strlen(user_ssh_file_name) + 5; // ".pub" + "\0"
			tmp_ssh_file = xmalloc(ssh_file_len);
			snprintf(tmp_ssh_file, ssh_file_len, "%s.pub", user_ssh_file_name);

			user_ssh_file_name = tmp_ssh_file;
		}

		if (strcmp(user_node, "name") == 0) {
			error = local_user_array_add_key(&ctx->local_users, user_name, user_ssh_file_name);
			// error = local_user_add_key(user_list, user_name, user_ssh_file_name);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "local_user_add_key error");
				return -1;
			}
		} else if (strcmp(user_node, "algorithm") == 0) {
			error = local_user_array_set_key_algorithm(&ctx->local_users, user_name, user_ssh_file_name, value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "local_user_add_algorithm error");
				return -1;
			}
		} else if (strcmp(user_node, "key-data") == 0) {
			error = local_user_array_set_key_data(&ctx->local_users, user_name, user_ssh_file_name, value);
			if (error != 0) {
				SRPLG_LOG_ERR(PLUGIN_NAME, "local_user_add_key_data error");
				return -1;
			}
		}
	}

	FREE_SAFE(tmp_xpath);

	if (tmp_ssh_file != NULL) {
		// in case there was no .pub extension
		// user_ssh_file_name points to tmp_ssh_file
		FREE_SAFE(user_ssh_file_name);
	}

	return 0;
}

#ifndef SYSTEMD
int set_dns_timeout(char *value)
{
	int err = 0;
	// load config and set timeout option in it
	rconf_t cfg;
	rconf_error_t rc_err = rconf_error_none;
	int timeout = 0;
	rconf_init(&cfg);

	rc_err = rconf_load_file(&cfg, RESOLV_CONF_PATH);
	if (rc_err != rconf_error_none) {
		goto err_out;
	}

	timeout = atoi(value);
	SRPLG_LOG_DBG(PLUGIN_NAME, "New timeout value: %d", timeout);

	rc_err = rconf_set_timeout(&cfg, timeout);
	if (rc_err) {
		goto err_out;
	}

	rc_err = rconf_export(&cfg, RESOLV_CONF_PATH);
	if (rc_err) {
		goto err_out;
	}

	goto out;

err_out:
	SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured with resolv.conf: (%d) -> %s\n", rc_err, rconf_error2str(rc_err));
out:
	rconf_free(&cfg);
	return err;
}

int set_dns_attempts(char *value)
{
	int err = 0;
	// load config and set attempts option in it
	rconf_t cfg;
	rconf_error_t rc_err = rconf_error_none;
	int attempts = 0;
	rconf_init(&cfg);
	rc_err = rconf_load_file(&cfg, RESOLV_CONF_PATH);

	if (rc_err != rconf_error_none) {
		goto err_out;
	}

	attempts = atoi(value);
	SRPLG_LOG_DBG(PLUGIN_NAME, "New attempts value: %d", attempts);

	rc_err = rconf_set_attempts(&cfg, attempts);
	if (rc_err) {
		goto err_out;
	}

	rc_err = rconf_export(&cfg, RESOLV_CONF_PATH);
	if (rc_err) {
		goto err_out;
	}

	goto out;

err_out:
	SRPLG_LOG_ERR(PLUGIN_NAME, "Error occured with resolv.conf: (%d) -> %s\n", rc_err, rconf_error2str(rc_err));
out:
	rconf_free(&cfg);
	return err;
}

#endif

int system_set_contact_info(const char *value)
{
	struct passwd *pwd = {0};
	FILE *tmp_pwf = NULL; // temporary passwd file
	int read_fd = -1;
	int write_fd = -1;
	struct stat stat_buf = {0};
	off_t offset = 0;

	// write /etc/passwd to a temp file
	// and change GECOS field for CONTACT_USERNAME
	tmp_pwf = fopen(CONTACT_TEMP_FILE, "w");
	if (!tmp_pwf) {
		goto fail;
	}

	endpwent(); // close the passwd db

	pwd = getpwent();
	if (pwd == NULL) {
		goto fail;
	}

	do {
		if (strcmp(pwd->pw_name, CONTACT_USERNAME) == 0) {
			// TODO: check max allowed len of gecos field
			pwd->pw_gecos = (char *) value;

			if (putpwent(pwd, tmp_pwf) != 0) {
				goto fail;
			}

		} else {
			if (putpwent(pwd, tmp_pwf) != 0) {
				goto fail;
			}
		}
	} while ((pwd = getpwent()) != NULL);

	fclose(tmp_pwf);
	tmp_pwf = NULL;

	// create a backup file of /etc/passwd
	if (rename(PASSWD_FILE, PASSWD_BAK_FILE) != 0) {
		goto fail;
	}

	// copy the temp file to /etc/passwd
	read_fd = open(CONTACT_TEMP_FILE, O_RDONLY);
	if (read_fd == -1) {
		goto fail;
	}

	if (fstat(read_fd, &stat_buf) != 0) {
		goto fail;
	}

	write_fd = open(PASSWD_FILE, O_WRONLY | O_CREAT, stat_buf.st_mode);
	if (write_fd == -1) {
		goto fail;
	}

	if (sendfile(write_fd, read_fd, &offset, (size_t) stat_buf.st_size) == -1) {
		goto fail;
	}

	// remove the temp file
	if (remove(CONTACT_TEMP_FILE) != 0) {
		goto fail;
	}

	close(read_fd);
	close(write_fd);

	return 0;

fail:
	// if copying tmp file to /etc/passwd failed
	// rename the backup back to passwd
	if (access(PASSWD_FILE, F_OK) != 0) {
		rename(PASSWD_BAK_FILE, PASSWD_FILE);
	}

	if (tmp_pwf != NULL) {
		fclose(tmp_pwf);
	}

	if (read_fd != -1) {
		close(read_fd);
	}

	if (write_fd != -1) {
		close(write_fd);
	}

	return -1;
}

int system_set_timezone(const char *value)
{
	int error = 0;
	char *zoneinfo = TIMEZONE_DIR; // not NULL terminated
	char *tz = NULL;

	tz = xmalloc(strnlen(zoneinfo, ZONE_DIR_LEN) + strnlen(value, TIMEZONE_NAME_LEN) + 1);

	strncpy(tz, zoneinfo, strnlen(zoneinfo, ZONE_DIR_LEN) + 1);
	strncat(tz, value, strnlen(value, TIMEZONE_NAME_LEN));

	// check if file exists in TIMEZONE_DIR
	if (access(tz, F_OK) != 0) {
		goto fail;
	}

	if (access(LOCALTIME_FILE, F_OK) == 0) {
		// if the /etc/localtime symlink file exists
		// unlink it
		error = unlink(LOCALTIME_FILE);
		if (error != 0) {
			goto fail;
		}
	} // if it doesn't, it will be created

	error = symlink(tz, LOCALTIME_FILE);
	if (error != 0)
		goto fail;

	FREE_SAFE(tz);
	return 0;

fail:
	FREE_SAFE(tz);
	return -1;
}

int system_set_datetime(char *datetime)
{
	struct tm t = {0};
	time_t time_to_set = 0;
	struct timespec stime = {0};

	/* datetime format must satisfy constraint:
		"\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(\.\d+)?(Z|[\+\-]\d{2}:\d{2})"
		currently only "%d-%d-%dT%d-%d-%dZ" is supported
		TODO: Add support for:
			- 2021-02-09T06:02:39.234+01:00
			- 2021-02-09T06:02:39.234Z
			- 2021-02-09T06:02:39+11:11
	*/

	if (strptime(datetime, "%FT%TZ", &t) == 0) {
		return -1;
	}

	time_to_set = mktime(&t);
	if (time_to_set == -1) {
		return -1;
	}

	stime.tv_sec = time_to_set;

	if (clock_settime(CLOCK_REALTIME, &stime) == -1) {
		return -1;
	}

	return 0;
}

int system_set_location(const char *location)
{
	long int error = -1;
	char *location_file_path = NULL;
	int fd = -1;
	size_t len = 0;

	location_file_path = system_get_plugin_file_path(LOCATION_FILENAME, false);
	if (location_file_path == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "set_location: couldn't get location file path");
		goto error_out;
	}

	fd = open(location_file_path, O_CREAT | O_WRONLY | O_TRUNC);
	if (fd == -1) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "set_location: couldn't open location file path");
		goto error_out;
	}

	len = strnlen(location, MAX_LOCATION_LENGTH);

	error = write(fd, location, len);
	if (error == -1) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "set_location: couldn't write to location file path");
		goto error_out;
	}

	error = close(fd);
	if (error == -1) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "set_location: couldn't close location file path");
		goto error_out;
	}

	fd = -1;

	FREE_SAFE(location_file_path);

	return 0;

error_out:
	if (location_file_path != NULL) {
		FREE_SAFE(location_file_path);
	}

	if (fd == -1) {
		close(fd);
	}

	return -1;
}

int system_ntp_set_server_name(char *name, char *address)
{
	FILE *fp = NULL;
	FILE *fp_tmp = NULL;
	char entry[NTP_MAX_ENTRY_LEN] = {0};
	char *ntp_names_file_path = NULL;
	char *tmp_ntp_names_file_path = NULL;
	char *line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	bool entry_updated = false;

	ntp_names_file_path = system_get_plugin_file_path(NTP_NAMES_FILENAME, false);
	if (ntp_names_file_path == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "ntp_set_server_name: couldn't get ntp_names file path");
		goto error_out;
	}

	if (strcmp(address, "") != 0) { // leave entry empty if it needs to be deleted
		if (snprintf(entry, NTP_MAX_ENTRY_LEN, "%s=%s\n", name, address) < 0) {
			goto error_out;
		}
	}

	tmp_ntp_names_file_path = system_get_plugin_file_path(NTP_TMP_NAMES_FILENAME, true);
	if (ntp_names_file_path == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "ntp_set_server_name: couldn't get tmp_ntp_names file path");
		goto error_out;
	}

	fp = fopen(ntp_names_file_path, "r");
	if (fp == NULL) {
		goto error_out;
	}

	fp_tmp = fopen(tmp_ntp_names_file_path, "a");
	if (fp_tmp == NULL) {
		goto error_out;
	}

	while ((read = getline(&line, &len, fp)) != -1) {
		// check if ntp server with name already exists
		if (strncmp(line, name, strlen(name)) == 0) {
			// update it
			fputs(entry, fp_tmp);
			entry_updated = true;
		} else {
			fputs(line, fp_tmp);
		}
	}

	FREE_SAFE(line);
	fclose(fp);
	fp = NULL;
	fclose(fp_tmp);
	fp_tmp = NULL;

	// rename the tmp file
	if (rename(tmp_ntp_names_file_path, ntp_names_file_path) != 0) {
		goto error_out;
	}

	FREE_SAFE(tmp_ntp_names_file_path);

	// if the current entry wasn't updated, append it
	if (!entry_updated) {
		fp = fopen(ntp_names_file_path, "a");
		if (fp == NULL) {
			goto error_out;
		}

		fputs(entry, fp);

		fclose(fp);
		fp = NULL;
	}

	FREE_SAFE(ntp_names_file_path);
	return 0;

error_out:
	if (ntp_names_file_path != NULL) {
		FREE_SAFE(ntp_names_file_path);
	}

	if (tmp_ntp_names_file_path != NULL) {
		FREE_SAFE(tmp_ntp_names_file_path);
	}

	if (fp != NULL) {
		fclose(fp);
	}

	if (fp_tmp != NULL) {
		fclose(fp_tmp);
	}

	return -1;
}

int system_ntp_get_server_name(char **name, char *address)
{
	int error = 0;
	FILE *fp = NULL;
	char *line = NULL;
	size_t len = 0;
	ssize_t read = 0;
	char *ntp_file_path = NULL;
	bool entry_found = false;

	ntp_file_path = system_get_plugin_file_path(NTP_NAMES_FILENAME, false);
	if (ntp_file_path == NULL) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "ntp_get_server_name: couldn't get ntp_names file path");
		goto error_out;
	}

	fp = fopen(ntp_file_path, "r");
	if (fp == NULL) {
		FREE_SAFE(ntp_file_path);
		goto error_out;
	}

	while ((read = getline(&line, &len, fp)) != -1) {
		// find name for given ntp server address
		if (strstr(line, address) != NULL) {
			// remove the newline char from line
			line[strlen(line) - 1] = '\0';

			char *tmp_name = strchr(line, '=');

			// "truncate" line buf by placing null term where '=' is
			*tmp_name = '\0';

			*name = xstrdup(line); // line contains the string before '=' now

			entry_found = true;

			break;
		}
	}

	if (!entry_found) {
		SRPLG_LOG_INF(PLUGIN_NAME, "No name in %s for ntp server with address %s was found", ntp_file_path, address);
		SRPLG_LOG_INF(PLUGIN_NAME, "Setting address %s as name...", address);

		*name = xstrdup(address);

		// save to file
		error = system_ntp_set_server_name(address, *name);
		if (error != 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "ntp_set_server_name error");
			goto error_out;
		}
	}

	FREE_SAFE(ntp_file_path);
	FREE_SAFE(line);
	fclose(fp);

	return 0;

error_out:
	if (ntp_file_path != NULL) {
		FREE_SAFE(ntp_file_path);
	}

	if (line != NULL) {
		FREE_SAFE(line);
	}

	if (fp != NULL) {
		fclose(fp);
	}

	return -1;
}

int system_ntp_set_entry_datastore(sr_session_ctx_t *session, ntp_server_t *server_entry)
{
	int error = 0;
	char ntp_path_buffer[PATH_MAX] = {0};
	char xpath_buffer[PATH_MAX] = {0};

	// setup the xpath
	// example xpath: 	"ietf-system:system/ntp/server[name='hr3.pool.ntp.org']/udp/address"
	error = snprintf(ntp_path_buffer, sizeof(ntp_path_buffer) / sizeof(char), "%s[name=\"%s\"]", NTP_SERVER_YANG_PATH, server_entry->name);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error");
		goto error_out;
	}

	// name
	error = sr_set_item_str(session, ntp_path_buffer, server_entry->name, NULL, SR_EDIT_DEFAULT);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	// address
	error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/udp/address", ntp_path_buffer);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error");
		goto error_out;
	}

	error = sr_set_item_str(session, xpath_buffer, server_entry->address, NULL, SR_EDIT_DEFAULT);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	// association type
	error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/association-type", ntp_path_buffer);
	if (error < 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error");
		goto error_out;
	}

	error = sr_set_item_str(session, xpath_buffer, server_entry->assoc_type, NULL, SR_EDIT_DEFAULT);
	if (error) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	// port, iburst and prefer can be NULL
	// port
	if (server_entry->port != NULL) {
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/udp/port", ntp_path_buffer);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error");
			goto error_out;
		}

		error = sr_set_item_str(session, xpath_buffer, server_entry->port, NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	// iburst
	if (server_entry->iburst != NULL) {
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/iburst", ntp_path_buffer);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error");
			goto error_out;
		}

		error = sr_set_item_str(session, xpath_buffer, (strlen(server_entry->iburst) > 0) ? "true" : "false", NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	// prefer
	if (server_entry->prefer != NULL) {
		error = snprintf(xpath_buffer, sizeof(xpath_buffer), "%s/prefer", ntp_path_buffer);
		if (error < 0) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "snprintf error");
			goto error_out;
		}

		error = sr_set_item_str(session, xpath_buffer, (strlen(server_entry->prefer) > 0) ? "true" : "false", NULL, SR_EDIT_DEFAULT);
		if (error) {
			SRPLG_LOG_ERR(PLUGIN_NAME, "sr_set_item_str error (%d): %s", error, sr_strerror(error));
			goto error_out;
		}
	}

	error = sr_apply_changes(session, 0);
	if (error != 0) {
		SRPLG_LOG_ERR(PLUGIN_NAME, "sr_apply_changes error (%d): %s", error, sr_strerror(error));
		goto error_out;
	}

	return 0;

error_out:
	return -1;
}