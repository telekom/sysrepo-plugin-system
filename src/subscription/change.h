#ifndef SYSTEM_PLUGIN_SUBSCRIPTION_CHANGE_H
#define SYSTEM_PLUGIN_SUBSCRIPTION_CHANGE_H

#include <sysrepo_types.h>

// basic system values //
int system_change_hostname(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);
int system_change_location(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);
int system_change_timezone_name(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);
int system_change_timezone_utc_offset(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);

// ntp //
int system_change_ntp_enabled(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);
int system_change_ntp_server(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);

// dns //
int system_change_dns_resolver_search(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);
int system_change_dns_resolver_server(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);
int system_change_dns_resolver_timeout(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);
int system_change_dns_resolver_attempts(sr_session_ctx_t *session, uint32_t subscription_id, const char *module_name, const char *xpath, sr_event_t event, uint32_t request_id, void *private_data);

#endif // SYSTEM_PLUGIN_SUBSCRIPTION_CHANGE_H