#include <sysrepo.h>
#include <string.h>
#ifdef SYSTEMD
#include <systemd/sd-bus.h>
#else
#include "utils/dns/resolv_conf.h"
#endif
#include "utils/dns/search.h"
#include "utils/memory.h"

enum dns_search_error_e {
	dns_search_error_none = 0,
	dns_search_error_MAX
};

typedef enum dns_search_error_e dns_search_error_t;

#ifdef SYSTEMD
typedef struct domain_s domain_t;
typedef struct domain_list_s domain_list_t;

struct domain_s {
	int ifindex;
	char *domain;
	int search;
};

struct domain_list_s {
	domain_t *list;
	int size;
};

// helper sd-bus using functions
static int get_domains(domain_list_t *list);
static int set_domains(domain_list_t *list);

// list functions
#define domain_is_valid(dm) ((dm)->domain != NULL)
static void domain_list_add(domain_list_t *list, domain_t dt);
static void domain_list_remove(domain_list_t *list, char *domain);
static void domain_list_free(domain_list_t *list);
#endif

int dns_search_add(char *search_val)
{
	int err = 0;
#ifdef SYSTEMD
	domain_list_t search_list = {0};

	err = get_domains(&search_list);

	if (err != 0) {
		goto out;
	}

	// got the list -> update it
	domain_list_add(&search_list, (domain_t){.search = 0, .ifindex = 0, .domain = search_val});
	err = set_domains(&search_list);

out:
	domain_list_free(&search_list);
#else
	rconf_t cfg;
	rconf_init(&cfg);
	rconf_error_t rc_err = rconf_error_none;

	rc_err = rconf_load_file(&cfg, RESOLV_CONF_PATH);
	if (rc_err != rconf_error_none) {
		goto err_out;
	}

	SRP_LOG_DBG("No error for now...\n");

	// file read normally -> add new search domain if possible
	rc_err = rconf_add_search(&cfg, search_val);
	if (rc_err != rconf_error_none) {
		goto err_out;
	}

	// finally write to the file
	rc_err = rconf_export(&cfg, RESOLV_CONF_PATH);
	if (rc_err != rconf_error_none) {
		goto err_out;
	}

	goto out;

err_out:
	SRP_LOG_ERR("Error adding search option '%s' to resolv.conf: (%d) => %s", search_val, rc_err, rconf_error2str(rc_err));
	err = (int) rc_err;

out:
	rconf_free(&cfg);
#endif
	return err;
}

const char *dns_search_error2str(int err)
{
	const char *errs[] = {
		[dns_search_error_none] = "No error",
	};
	const char *unk = "Unknown error code";
	if (err >= dns_search_error_none && err < dns_search_error_MAX) {
		return errs[err];
	}
	return unk;
}

int dns_search_remove(char *search_val)
{
	int err = 0;
#ifdef SYSTEMD
	domain_list_t search_list = {0};
	err = get_domains(&search_list);
	if (err != 0) {
		goto out;
	}

	// remove the given list
	domain_list_remove(&search_list, search_val);

	err = set_domains(&search_list);

out:
	domain_list_free(&search_list);
#else
	rconf_t cfg;
	rconf_init(&cfg);
	rconf_error_t rc_err = rconf_error_none;

	rc_err = rconf_load_file(&cfg, RESOLV_CONF_PATH);
	if (rc_err != rconf_error_none) {
		goto err_out;
	}

	// file read normally -> remove search
	rc_err = rconf_remove_search(&cfg, search_val);
	if (rc_err != rconf_error_none) {
		goto err_out;
	}

	// finally write to the file
	rc_err = rconf_export(&cfg, RESOLV_CONF_PATH);
	if (rc_err != rconf_error_none) {
		goto err_out;
	}

	goto out;

err_out:
	SRP_LOG_ERR("Error adding search option '%s' to resolv.conf: (%d) => %s", search_val, rc_err, rconf_error2str(rc_err));
	err = (int) rc_err;

out:
	rconf_free(&cfg);
#endif
	return err;
}

#ifdef SYSTEMD
int get_domains(domain_list_t *list)
{
	int err = 0;
	domain_t domain_value = {0};
	int r;
	sd_bus_message *msg = NULL;
	sd_bus_error sdb_err = SD_BUS_ERROR_NULL;
	sd_bus *bus = NULL;

	r = sd_bus_open_system(&bus);
	if (r < 0) {
		SRP_LOG_ERR("Failed to open system bus: %s\n", strerror(-r));
		goto finish;
	}

	r = sd_bus_get_property(
		bus,
		"org.freedesktop.resolve1",
		"/org/freedesktop/resolve1",
		"org.freedesktop.resolve1.Manager",
		"Domains",
		&sdb_err,
		&msg,
		"a(isb)");

	if (r < 0) {
		goto invalid;
	}

	// message recieved -> enter msg and get needed info
	r = sd_bus_message_enter_container(msg, 'a', "(isb)");
	if (r < 0) {
		goto invalid;
	}

	for (;;) {
		r = sd_bus_message_enter_container(msg, 'r', "isb");
		if (r < 0) {
			goto invalid;
		}
		if (r == 0) {
			break;
		}
		r = sd_bus_message_read(msg, "isb", &domain_value.ifindex, &domain_value.domain, &domain_value.search);
		if (r < 0) {
			goto invalid;
		}

		r = sd_bus_message_exit_container(msg);
		if (r < 0) {
			goto invalid;
		}

		// only used value is 's' part of the struct -> see about others later
		// got the value -> append to list
		domain_list_add(list, domain_value);
	}

	goto finish;

invalid:
	SRP_LOG_ERR("sd-bus failure: %d, sdb_err contents: '%s'", r, sdb_err.message);
	err = -1;

finish:
	sd_bus_message_unref(msg);
	sd_bus_flush_close_unref(bus);
	return err;
}

static int set_domains(domain_list_t *list)
{
	int err = 0;
	int r;
	sd_bus_error sdb_err = SD_BUS_ERROR_NULL;
	sd_bus_message *msg = NULL;
	sd_bus_message *reply = NULL;
	sd_bus *bus = NULL;

	r = sd_bus_open_system(&bus);
	if (r < 0) {
		SRP_LOG_ERR("Failed to open system bus: %s\n", strerror(-r));
		goto invalid;
	}

	r = sd_bus_message_new_method_call(
		bus,
		&msg,
		"org.freedesktop.resolve1",
		"/org/freedesktop/resolve1",
		"org.freedesktop.resolve1.Manager",
		"SetLinkDomains");
	if (r < 0) {
		goto invalid;
	}

	// set ifindex to the first value in the list
	r = sd_bus_message_append(msg, "i", SYSTEMD_IFINDEX);
	if (r < 0) {
		goto invalid;
	}

	r = sd_bus_message_open_container(msg, 'a', "(sb)");
	if (r < 0) {
		goto invalid;
	}

	for (int i = 0; i < list->size; i++) {
		domain_t *dptr = list->list + i;
		// check if the domain was deleted
		if (domain_is_valid(dptr)) {
			r = sd_bus_message_append(msg, "(sb)", dptr->domain, dptr->search);
			if (r < 0) {
				goto invalid;
			}
		}
	}

	r = sd_bus_message_close_container(msg);
	if (r < 0) {
		goto invalid;
	}

	r = sd_bus_call(bus, msg, 0, &sdb_err, &reply);
	if (r < 0) {
		goto invalid;
	}

	SRP_LOG_INF("Set domains successfully!");
	goto finish;

invalid:
	SRP_LOG_ERR("sd-bus failure: %d, sdb_err contents: '%s'", r, sdb_err.message);

finish:
	sd_bus_message_unref(msg);
	sd_bus_flush_close_unref(bus);
	return err;
}

static void domain_list_add(domain_list_t *list, domain_t dt)
{
	list->size += 1;
	list->list = (domain_t *) xrealloc(list->list, sizeof(domain_t) * (unsigned long) (list->size));
	list->list[list->size - 1] = (domain_t){
		.search = dt.search,
		.ifindex = dt.ifindex,
		.domain = xstrdup(dt.domain),
	};
}

static void domain_list_remove(domain_list_t *list, char *domain)
{
	const unsigned long n = strlen(domain);
	for (int i = 0; i < list->size; i++) {
		if (strncmp(list->list[i].domain, domain, n) == 0) {
			FREE_SAFE(list->list[i].domain);
			break;
		}
	}
}

static void domain_list_free(domain_list_t *list)
{
	for (int i = 0; i < list->size; i++) {
		if (domain_is_valid(&list->list[i])) {
			FREE_SAFE(list->list[i].domain);
		}
	}
	FREE_SAFE(list->list);
}
#endif
