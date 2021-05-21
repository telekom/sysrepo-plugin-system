#ifndef DNS_RESOLV_CONF_H
#define DNS_RESOLV_CONF_H

#include <stdio.h>
#include <bits/types/res_state.h>

#define RESOLV_CONF_FILE "/etc/resolv.conf"

enum rconf_error_e {
	rconf_error_none = 0,
	rconf_error_loading_file,
	rconf_error_alloc,
	rconf_error_invalid_format,
	rconf_error_nameserver,
	rconf_error_search,
	rconf_error_sortlist,
	rconf_error_options_ndots,
	rconf_error_options_timeout,
	rconf_error_options_attempts,
};

typedef struct rconf_s rconf_t;
typedef struct rconf_line_s rconf_line_t;
typedef struct rconf_options_s rconf_options_t;
typedef enum rconf_error_e rconf_error_t;

struct rconf_options_s {
	int ndots;
	int timeout;
	int attempts;
};

struct rconf_s {
	// nameservers
	char *nameserver[MAXNS];
	int nameserver_n;

	// search list
	char *search[MAXDNSRCH];
	int search_n;

	// sortlist
	char *sortlist[MAXRESOLVSORT];
	int sortlist_n;

	// options
	rconf_options_t options;
};

void rconf_init(rconf_t *cfg);
rconf_error_t rconf_load_file(rconf_t *cfg, const char *fpath);
rconf_error_t rconf_export(rconf_t *cfg, const char *fpath);
rconf_error_t rconf_add_nameserver(rconf_t *cfg, char *nameserver);
rconf_error_t rconf_add_search(rconf_t *cfg, char *search);
rconf_error_t rconf_set_ndots(rconf_t *cfg, int ndots);
rconf_error_t rconf_set_timeout(rconf_t *cfg, int timeout);
rconf_error_t rconf_set_attempts(rconf_t *cfg, int attempts);
void rconf_print(rconf_t *cfg, FILE *fptr);
void rconf_free(rconf_t *cfg);

#endif // DNS_RESOLV_CONF_H
