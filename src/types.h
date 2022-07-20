#ifndef SYSTEM_PLUGIN_TYPES_H
#define SYSTEM_PLUGIN_TYPES_H

// DNS

typedef struct system_ntp_server_s system_ntp_server_t;
typedef struct system_ntp_server_element_s system_ntp_server_element_t;
typedef struct system_dns_search_s system_dns_search_t;
typedef struct system_dns_search_element_s system_dns_search_element_t;
typedef struct system_dns_server_s system_dns_server_t;
typedef struct system_dns_server_element_s system_dns_server_element_t;
typedef struct system_ip_address_s system_ip_address_t;
typedef union system_ip_address_value_u system_ip_address_value_t;
typedef struct system_local_user_s system_local_user_t;
typedef struct system_local_user_element_s system_local_user_element_t;
typedef struct system_authorized_key_s system_authorized_key_t;
typedef struct system_authorized_key_element_s system_authorized_key_element_t;

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

struct system_ntp_server_s {
	char *name;
	char *address;
	char *port;
	char *association_type;
	char *iburst;
	char *prefer;
};

struct system_ntp_server_element_s {
	system_ntp_server_t server;
	struct system_ntp_server_element_s *next;
};

struct system_dns_search_s {
	char *domain;
	int ifindex;
	int search;
};

struct system_dns_server_s {
	char *name;
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

struct system_local_user_s {
	char *name;
	char *password;
	system_authorized_key_element_t *key_head;
};

struct system_local_user_element_s {
	system_local_user_t user;
	system_local_user_element_t *next;
};

struct system_authorized_key_s {
	char *name;
	char *algorithm;
	char *data;
};

struct system_authorized_key_element_s {
	system_authorized_key_t key;
	system_authorized_key_element_t *next;
};

#endif // SYSTEM_PLUGIN_TYPES_H