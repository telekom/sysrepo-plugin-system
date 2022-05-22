#ifndef SYSTEM_PLUGIN_SUBSCRIPTION_OPERATIONAL_H
#define SYSTEM_PLUGIN_SUBSCRIPTION_OPERATIONAL_H

#include <sysrepo_types.h>

// platform //
int system_operational_platform(sr_session_ctx_t *session, uint32_t sub_id, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);

// clock //
int system_operational_clock(sr_session_ctx_t *session, uint32_t sub_id, const char *module_name, const char *path, const char *request_xpath, uint32_t request_id, struct lyd_node **parent, void *private_data);

#endif // SYSTEM_PLUGIN_SUBSCRIPTION_OPERATIONAL_H