/*
 * telekom / sysrepo-plugin-system
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2022 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "authorized_key.h"
#include <stdlib.h>
#include <string.h>

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

	if (name) {
		key->name = strdup(name);
		return key->name == NULL;
	}

	return 0;
}

int system_authorized_key_set_algorithm(system_authorized_key_t *key, const char *algorithm)
{
	if (key->algorithm) {
		free(key->algorithm);
		key->algorithm = 0;
	}

	if (algorithm) {
		key->algorithm = strdup(algorithm);
		return key->algorithm == NULL;
	}

	return 0;
}

int system_authorized_key_set_data(system_authorized_key_t *key, const char *data)
{
	if (key->data) {
		free(key->data);
		key->data = 0;
	}

	if (data) {
		key->data = strdup(data);
		return key->data == NULL;
	}

	return 0;
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

int system_authorized_key_cmp_fn(const void *e1, const void *e2)
{
	system_authorized_key_t *k1 = (system_authorized_key_t *) e1;
	system_authorized_key_t *k2 = (system_authorized_key_t *) e2;

	return strcmp(k1->name, k2->name);
}