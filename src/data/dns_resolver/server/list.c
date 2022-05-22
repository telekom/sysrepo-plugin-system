#include "list.h"
#include "data/dns_resolver/server.h"
#include "utils/memory.h"

#include <utlist.h>

void system_dns_server_list_init(system_dns_server_element_t **head)
{
	*head = NULL;
}

int system_dns_server_list_add(system_dns_server_element_t **head, system_dns_server_t server)
{
	system_dns_server_element_t *new_el = (system_dns_server_element_t *) malloc(sizeof(system_dns_server_element_t));

	if (!new_el) {
		return -1;
	}

	// set given value
	new_el->server = server;

	// add to list
	LL_APPEND(*head, new_el);

	return 0;
}

void system_dns_server_list_free(system_dns_server_element_t **head)
{
	system_dns_server_element_t *iter_el = NULL, *tmp_el = NULL;

	LL_FOREACH_SAFE(*head, iter_el, tmp_el)
	{
		LL_DELETE(*head, iter_el);
		system_dns_server_free(&iter_el->server);
		free(iter_el);
	}

	system_dns_server_list_init(head);
}