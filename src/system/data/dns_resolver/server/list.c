#include "list.h"
#include "system/data/dns_resolver/server.h"

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

	// copy value
	system_dns_server_init(&new_el->server);
	system_dns_server_set_name(&new_el->server, server.name);
	system_dns_server_set_address(&new_el->server, server.address);
	system_dns_server_set_port(&new_el->server, server.port);

	// add to list
	LL_APPEND(*head, new_el);

	return 0;
}

int system_dns_server_element_cmp_fn(void *e1, void *e2)
{
	system_dns_server_element_t *s1 = (system_dns_server_element_t *) e1;
	system_dns_server_element_t *s2 = (system_dns_server_element_t *) e2;

	return strcmp(s1->server.name, s2->server.name);
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