#include <utils/dns/search.h>
#include <utils/memory.h>
#include <sysrepo.h>
#ifdef SYSTEMD
#include <systemd/sd-bus.h>
#else
#endif

enum dns_search_error_e {
	dns_search_error_none = 0,
	dns_search_error_MAX
};

typedef enum dns_search_error_e dns_search_error_t;
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

// helper functions
static int get_domains(domain_list_t *list);
static int set_domains(domain_list_t *list);

// list functions
static void domain_list_add(domain_list_t *list, domain_t dt);
static void domain_list_free(domain_list_t *list);

int dns_search_add(char *search_val)
{
	int err = 0;
	domain_list_t search_list = {0};

	err = get_domains(&search_list);

	if (err != 0) {
		goto out;
	}

	SRP_LOG_DBG("DNS Domains:");
	for (int i = 0; i < search_list.size; i++) {
		SRP_LOG_DBG("DNS Domains list item #%d: '%s'", i + 1, search_list.list[i].domain);
	}

	// got the list -> update it
	domain_list_add(&search_list, (domain_t){.search = 0, .ifindex = 0, .domain = search_val});
	err = set_domains(&search_list);

out:
	domain_list_free(&search_list);
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
	return err;
}

int get_domains(domain_list_t *list)
{
	int err = 0;
	domain_t domain_value = {0};

#ifdef SYSTEMD
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

finish:
	sd_bus_message_unref(msg);
	sd_bus_flush_close_unref(bus);
#else
#endif
	return err;
}

static int set_domains(domain_list_t *list)
{
	int err = 0;
#ifdef SYSTEMD
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
	r = sd_bus_message_append(msg, "i", list->list[0].ifindex);
	if (r < 0) {
		goto invalid;
	}

	r = sd_bus_message_open_container(msg, 'a', "(sb)");
	if (r < 0) {
		goto invalid;
	}

	for (int i = 0; i < list->size; i++) {
		domain_t *dptr = list->list + i;
		r = sd_bus_message_append(msg, "(sb)", dptr->domain, dptr->search);
		if (r < 0) {
			goto invalid;
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
#else
#endif
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

static void domain_list_free(domain_list_t *list)
{
	for (int i = 0; i < list->size; i++) {
		FREE_SAFE(list->list[i].domain);
	}
	FREE_SAFE(list->list);
}
