#ifndef DNS_SERVER_H
#define DNS_SERVER_H

#include <bits/types/res_state.h>
#include <stdbool.h>

typedef struct ip_addr_s ip_addr_t;
typedef struct dns_server_s dns_server_t;
typedef struct dns_server_list_s dns_server_list_t;
typedef union ip_addr_value_u ip_addr_value_t;

union ip_addr_value_u {
	uint8_t ip4[4];
	uint8_t ip6[16];
};

struct ip_addr_s {
#ifdef SYSTEMD
	int af;
	ip_addr_value_t value;
#else
	char *value;
#endif
};

struct dns_server_s {
	char *name;
	ip_addr_t addr;
	int port;
	bool delete;
};

struct dns_server_list_s {
	dns_server_t *list;
	int size;
};

// server functions
void dns_server_init(dns_server_t *s);
void dns_server_set_name(dns_server_t *s, char *name);
int dns_server_set_address(dns_server_t *s, char *addr);
void dns_server_set_port(dns_server_t *s, int port);
void dns_server_free(dns_server_t *s);

// server list functions
void dns_server_list_init(dns_server_list_t *sl);
int dns_server_list_add_server(dns_server_list_t *sl, char *name);
int dns_server_list_set_server_delete(dns_server_list_t *sl, char *name);
int dns_server_list_set_address(dns_server_list_t *sl, char *name, char *address);
int dns_server_list_set_port(dns_server_list_t *sl, char *name, int port);
int dns_server_list_dump_config(dns_server_list_t *sl);
void dns_server_list_free(dns_server_list_t *sl);

#endif // DNS_SERVER_H
