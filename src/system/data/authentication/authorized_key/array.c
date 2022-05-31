#include "array.h"
#include "system/data/authentication/authorized_key.h"
#include "common.h"

static void system_authorized_key_copy_fn(void *dst, const void *src);
static void system_authorized_key_dtor_fn(void *elt);

void system_authorized_key_array_init(UT_array **keys)
{
	*keys = NULL;

	UT_icd keys_icd = {
		.sz = sizeof(system_authorized_key_t),
		.init = NULL,
		.copy = system_authorized_key_copy_fn,
		.dtor = system_authorized_key_dtor_fn,
	};

	utarray_new(*keys, &keys_icd);
	utarray_reserve(*keys, SYSTEM_AUTHORIZED_KEYS_MAX_COUNT);
}

int system_authorized_key_array_add(UT_array **keys, system_authorized_key_t key)
{
	int error = 0;

	if (utarray_len(*keys) >= SYSTEM_AUTHORIZED_KEYS_MAX_COUNT) {
		return -1;
	}

	utarray_push_back(*keys, &key);
	utarray_sort(*keys, system_authorized_key_cmp_fn);

	return error;
}

void system_authorized_key_array_free(UT_array **keys)
{
	utarray_free(*keys);
	*keys = NULL;
}

static void system_authorized_key_copy_fn(void *dst, const void *src)
{
	system_authorized_key_t *d = (system_authorized_key_t *) dst;
	system_authorized_key_t *s = (system_authorized_key_t *) src;

	system_authorized_key_init(d);
	system_authorized_key_set_name(d, s->name);

	if (s->algorithm) {
		system_authorized_key_set_algorithm(d, s->algorithm);
	}

	if (s->data) {
		system_authorized_key_set_data(d, s->data);
	}
}

static void system_authorized_key_dtor_fn(void *elt)
{
	system_authorized_key_free(elt);
}