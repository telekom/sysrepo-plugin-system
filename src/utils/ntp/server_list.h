#ifndef NTP_SERVER_LIST_H_ONCE
#define NTP_SERVER_LIST_H_ONCE
#include <inttypes.h>
#include <stdbool.h>
#include <sysrepo.h>

#define NTP_MAX_SERVERS 20 // TODO: update if needed
#define NTP_CONFIG_FILE "/etc/ntp.conf"
#define NTP_TEMP_FILE "/tmp/tmp_ntp.conf"
#define NTP_BAK_FILE "/etc/ntp.conf.bak"
#define NTP_MAX_ADDR_LEN 45
#define NTP_MAX_PORT_LEN 5
#define NTP_MAX_IBURST_LEN 6
#define NTP_MAX_PREFER_LEN 6
#define NTP_MAX_ENTRY_LEN 100

typedef struct ntp_server_list_s ntp_server_list_t;
typedef struct ntp_server_s ntp_server_t;

struct ntp_server_s {
	char *name;
	char *address;
	char *port;
	char *assoc_type;
	char *iburst;
	char *prefer;
	bool delete;
};

struct ntp_server_list_s {
	ntp_server_t servers[NTP_MAX_SERVERS];
	uint8_t count;
};

extern int ntp_get_server_name(char **name, char *address);
extern int ntp_set_entry_datastore(sr_session_ctx_t *session, ntp_server_t *server_entry);

int ntp_server_list_init(sr_session_ctx_t *session, ntp_server_list_t **sl);
int ntp_server_list_add_existing_servers(sr_session_ctx_t *session, ntp_server_list_t *sl);
int ntp_parse_config(ntp_server_t *server_entry, char *line);
int ntp_add_server_entry_to_list(ntp_server_list_t *sl, ntp_server_t *server_entry);
int ntp_server_list_add_server(ntp_server_list_t *sl, char *name);
int ntp_server_list_set_address(ntp_server_list_t *sl, char *name, char *address);
int ntp_server_list_set_port(ntp_server_list_t *sl, char *name, char *port);
int ntp_server_list_set_assoc_type(ntp_server_list_t *sl, char *name, char *assoc_type);
int ntp_server_list_set_iburst(ntp_server_list_t *sl, char *name, char *iburst);
int ntp_server_list_set_prefer(ntp_server_list_t *sl, char *name, char *prefer);
int ntp_server_list_set_delete(ntp_server_list_t *sl, char *name, bool delete_val);
void ntp_server_list_free(ntp_server_list_t *sl);
int save_ntp_config(ntp_server_list_t *sl);

#endif /* NTP_SERVER_LIST_H_ONCE */
