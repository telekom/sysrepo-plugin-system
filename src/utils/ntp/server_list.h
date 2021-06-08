#ifndef NTP_SERVER_LIST_H_ONCE
#define NTP_SERVER_LIST_H_ONCE
#include <inttypes.h>
#include <stdbool.h>

typedef struct ntp_server_list_s ntp_server_list_t;
typedef struct ntp_server_s ntp_server_t;

int ntp_server_list_init(ntp_server_list_t **sl);
int ntp_server_list_add_existing_servers(ntp_server_list_t *sl);
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
