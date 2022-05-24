#include "authorized_key.h"
#include "utils/memory.h"
#include <stdlib.h>

void system_authorized_key_init(system_authorized_key_t *key)
{
	*key = (system_authorized_key_t){0};
}

int system_authorized_key_set_name(system_authorized_key_t *key, const char *name)
{
	if (key->name) {
		free(key->name);
		key->name = 0;
	}

	key->name = xstrdup(name);

	return key->name != NULL;
}

int system_authorized_key_set_algorithm(system_authorized_key_t *key, const char *algorithm)
{
	if (key->algorithm) {
		free(key->algorithm);
		key->algorithm = 0;
	}

	key->algorithm = xstrdup(algorithm);

	return key->algorithm != NULL;
}

int system_authorized_key_set_data(system_authorized_key_t *key, const char *data)
{
	if (key->data) {
		free(key->data);
		key->data = 0;
	}

	key->data = xstrdup(data);

	return key->data != NULL;
}

void system_authorized_key_free(system_authorized_key_t *key)
{
	if (key->name) {
		free(key->name);
	}

	if (key->algorithm) {
		free(key->algorithm);
	}

	if (key->data) {
		free(key->data);
	}

	system_authorized_key_init(key);
}