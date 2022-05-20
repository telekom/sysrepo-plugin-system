#ifndef SYSTEM_PLUGIN_COMMON_H
#define SYSTEM_PLUGIN_COMMON_H

#define PLUGIN_NAME "system-plugin"

#define BASE_YANG_MODEL "ietf-system"

#define SYSTEM_SYSTEM_CONTAINER_YANG_PATH "/" BASE_YANG_MODEL ":system"

// rpc
#define SYSTEM_SET_CURRENT_DATETIME_RPC_YANG_PATH "/" BASE_YANG_MODEL ":set-current-datetime"
#define SYSTEM_RESTART_RPC_YANG_PATH "/" BASE_YANG_MODEL ":system-restart"
#define SYSTEM_SHUTDOWN_RPC_YANG_PATH "/" BASE_YANG_MODEL ":system-shutdown"

// operational
#define SYSTEM_STATE_YANG_PATH "/" BASE_YANG_MODEL ":system-state"
#define SYSTEM_STATE_PLATFORM_YANG_PATH SYSTEM_STATE_YANG_PATH "/platform"
#define SYSTEM_STATE_PLATFORM_OS_NAME_YANG_PATH SYSTEM_STATE_PLATFORM_YANG_PATH "/os-name"
#define SYSTEM_STATE_PLATFORM_OS_RELEASE_YANG_PATH SYSTEM_STATE_PLATFORM_YANG_PATH "/os-release"
#define SYSTEM_STATE_PLATFORM_OS_VERSION_YANG_PATH SYSTEM_STATE_PLATFORM_YANG_PATH "/os-version"
#define SYSTEM_STATE_PLATFORM_OS_MACHINE_YANG_PATH SYSTEM_STATE_PLATFORM_YANG_PATH "/machine"

#define SYSTEM_STATE_CLOCK_YANG_PATH SYSTEM_STATE_YANG_PATH "/clock"
#define SYSTEM_STATE_CLOCK_CURRENT_DATETIME_YANG_PATH SYSTEM_STATE_CLOCK_YANG_PATH "/current-datetime"
#define SYSTEM_STATE_CLOCK_BOOT_DATETIME_YANG_PATH SYSTEM_STATE_CLOCK_YANG_PATH "/boot-datetime"

// system //
#define SYSTEM_CONTACT_YANG_PATH SYSTEM_SYSTEM_CONTAINER_YANG_PATH "/contact"
#define SYSTEM_HOSTNAME_YANG_PATH SYSTEM_SYSTEM_CONTAINER_YANG_PATH "/hostname"
#define SYSTEM_LOCATION_YANG_PATH SYSTEM_SYSTEM_CONTAINER_YANG_PATH "/location"

// clock //
#define SYSTEM_TIMEZONE_NAME_YANG_PATH SYSTEM_SYSTEM_CONTAINER_YANG_PATH "/clock/timezone-name"
#define SYSTEM_TIMEZONE_UTC_OFFSET_YANG_PATH SYSTEM_SYSTEM_CONTAINER_YANG_PATH "/clock/timezone-utc-offset"

// ntp //
#define SYSTEM_NTP_ENABLED_YANG_PATH SYSTEM_SYSTEM_CONTAINER_YANG_PATH "/ntp/enabled"
#define SYSTEM_NTP_SERVER_YANG_PATH SYSTEM_SYSTEM_CONTAINER_YANG_PATH "/ntp/server"

// dns-resolver //
#define SYSTEM_DNS_RESOLVER_SEARCH_YANG_PATH SYSTEM_SYSTEM_CONTAINER_YANG_PATH "/dns-resolver/search"
#define SYSTEM_DNS_RESOLVER_SERVER_YANG_PATH SYSTEM_SYSTEM_CONTAINER_YANG_PATH "/dns-resolver/server"
#define SYSTEM_DNS_RESOLVER_TIMEOUT_YANG_PATH SYSTEM_SYSTEM_CONTAINER_YANG_PATH "/dns-resolver/options/timeout"
#define SYSTEM_DNS_RESOLVER_ATTEMPTS_YANG_PATH SYSTEM_SYSTEM_CONTAINER_YANG_PATH "/dns-resolver/options/attempts"

// authentication //
#define SYSTEM_AUTHENTICATION_USER_AUTHENTICATION_ORDER_YANG_PATH SYSTEM_SYSTEM_CONTAINER_YANG_PATH "/authentication/user-authentication-order"
#define SYSTEM_AUTHENTICATION_USER_YANG_PATH SYSTEM_SYSTEM_CONTAINER_YANG_PATH "/authentication/user"

#define SYSTEM_DATETIME_BUFFER_SIZE 30
#define SYSTEM_UTS_LEN 64

#define SYSTEM_TIMEZONE_DIR "/usr/share/zoneinfo"
#define SYSTEM_LOCALTIME_FILE "/etc/localtime"

#define SYSTEM_HOSTNAME_LENGTH_MAX 64
#define SYSTEM_TIMEZONE_NAME_LENGTH_MAX (14 * 3)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

typedef struct system_dns_search_s system_dns_search_t;
typedef struct system_dns_search_element_s system_dns_search_element_t;
typedef struct system_dns_server_s system_dns_server_t;
typedef struct system_dns_server_element_s system_dns_server_element_t;
typedef struct system_ip_address_s system_ip_address_t;
typedef union system_ip_address_value_u system_ip_address_value_t;

union system_ip_address_value_u {
	unsigned char v4[4];
	unsigned char v6[16];
};

struct system_ip_address_s {
#ifdef SYSTEMD
	int family;
	system_ip_address_value_t value;
#else
	const char *value;
#endif
};

struct system_dns_search_s {
	const char *domain;
	int ifindex;
	int search;
};

struct system_dns_server_s {
	const char *name;
	system_ip_address_t address;
	int port;
};

struct system_dns_search_element_s {
	system_dns_search_t search;
	struct system_dns_search_element_s *next;
};

struct system_dns_server_element_s {
	system_dns_server_t server;
	struct system_dns_server_element_s *next;
};

#endif // SYSTEM_PLUGIN_COMMON_H