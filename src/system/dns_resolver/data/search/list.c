#include "list.h"
#include "types.h"
#include "utils/memory.h"
#include "system/dns_resolver/data/search.h"

#include <utlist.h>

void system_dns_search_list_init(system_dns_search_element_t **head)
{
	*head = NULL;
}

int system_dns_search_list_add(system_dns_search_element_t **head, system_dns_search_t search)
{
	system_dns_search_element_t *new_el = (system_dns_search_element_t *) malloc(sizeof(system_dns_search_element_t));

	if (!new_el) {
		return -1;
	}

	// copy value

	// TODO: error checking
	system_dns_search_init(&new_el->search);
	system_dns_search_set_domain(&new_el->search, search.domain);
	system_dns_search_set_ifindex(&new_el->search, search.ifindex);
	system_dns_search_set_search(&new_el->search, search.search);

	// add to list
	LL_APPEND(*head, new_el);

	return 0;
}

void system_dns_search_list_free(system_dns_search_element_t **head)
{
	system_dns_search_element_t *iter_el = NULL, *tmp_el = NULL;

	LL_FOREACH_SAFE(*head, iter_el, tmp_el)
	{
		LL_DELETE(*head, iter_el);
		system_dns_search_free(&iter_el->search);
		free(iter_el);
	}

	system_dns_search_list_init(head);
}
