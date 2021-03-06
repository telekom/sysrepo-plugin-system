#include "search.h"
#include "utils/memory.h"

void system_dns_search_init(system_dns_search_t *search)
{
	*search = (system_dns_search_t){0};
}

int system_dns_search_set_domain(system_dns_search_t *search, const char *domain)
{
	int error = 0;

	if (search->domain) {
		free((void *) search->domain);
	}

	search->domain = xstrdup(domain);

	return error;
}

int system_dns_search_set_ifindex(system_dns_search_t *search, int ifindex)
{
	int error = 0;

	search->ifindex = ifindex;

	return error;
}

int system_dns_search_set_search(system_dns_search_t *search, int s)
{
	int error = 0;

	search->search = s;

	return error;
}

void system_dns_search_free(system_dns_search_t *search)
{
	if (search->domain) {
		free((void *) search->domain);
	}
	system_dns_search_init(search);
}
