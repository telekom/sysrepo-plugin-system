#include "list.h"
#include "system/data/ntp/server.h"
#include "types.h"

#include <utlist.h>

void system_ntp_server_list_init(system_ntp_server_element_t **head)
{
	*head = NULL;
}

int system_ntp_server_list_add(system_ntp_server_element_t **head, system_ntp_server_t server)
{
	system_ntp_server_element_t *new_el = (system_ntp_server_element_t *) malloc(sizeof(system_ntp_server_element_t));

	if (!new_el) {
		return -1;
	}

	// copy value
	system_ntp_server_init(&new_el->server);
	system_ntp_server_set_name(&new_el->server, server.name);
	system_ntp_server_set_address(&new_el->server, server.address);
	system_ntp_server_set_port(&new_el->server, server.port);
	system_ntp_server_set_association_type(&new_el->server, server.association_type);
	system_ntp_server_set_iburst(&new_el->server, server.iburst);
	system_ntp_server_set_prefer(&new_el->server, server.prefer);

	// add to list
	LL_APPEND(*head, new_el);

	return 0;
}

system_ntp_server_element_t *system_ntp_server_list_find(system_ntp_server_element_t *head, const char *name)
{
	system_ntp_server_element_t *found = NULL;
	system_ntp_server_element_t el = {
		.server = {
			.name = (char *) name,
		},
	};

	LL_SEARCH(head, found, &el, system_ntp_server_element_cmp_fn);

	return found;
}

int system_ntp_server_list_remove(system_ntp_server_element_t **head, const char *name)
{
	system_ntp_server_element_t *found = system_ntp_server_list_find(*head, name);

	if (!found) {
		return -1;
	}

	// remove and free found element
	LL_DELETE(*head, found);
	system_ntp_server_free(&found->server);
	free(found);

	return 0;
}

int system_ntp_server_element_cmp_fn(void *e1, void *e2)
{
	system_ntp_server_element_t *s1 = (system_ntp_server_element_t *) e1;
	system_ntp_server_element_t *s2 = (system_ntp_server_element_t *) e2;

	return strcmp(s1->server.name, s2->server.name);
}

int system_ntp_server_element_address_cmp_fn(void *e1, void *e2)
{
	system_ntp_server_element_t *s1 = (system_ntp_server_element_t *) e1;
	system_ntp_server_element_t *s2 = (system_ntp_server_element_t *) e2;

	return strcmp(s1->server.address, s2->server.address);
}

void system_ntp_server_list_free(system_ntp_server_element_t **head)
{
	system_ntp_server_element_t *iter_el = NULL, *tmp_el = NULL;

	LL_FOREACH_SAFE(*head, iter_el, tmp_el)
	{
		LL_DELETE(*head, iter_el);
		system_ntp_server_free(&iter_el->server);
		free(iter_el);
	}

	system_ntp_server_list_init(head);
}