#include "array.h"
#include "system/authentication/data/local_user.h"
#include "common.h"

static void system_local_user_copy_fn(void *dst, const void *src);
static void system_local_user_dtor_fn(void *elt);

void system_local_user_array_init(UT_array **users)
{
	*users = NULL;

	UT_icd users_icd = {
		.sz = sizeof(system_local_user_t),
		.init = NULL,
		.copy = system_local_user_copy_fn,
		.dtor = system_local_user_dtor_fn,
	};

	utarray_new(*users, &users_icd);
	utarray_reserve(*users, SYSTEM_LOCAL_USERS_MAX_COUNT);
}

int system_local_user_array_add(UT_array **users, system_local_user_t user)
{
	int error = 0;

	if (utarray_len(*users) >= SYSTEM_LOCAL_USERS_MAX_COUNT) {
		return -1;
	}

	utarray_push_back(*users, &user);
	utarray_sort(*users, system_local_user_cmp_fn);

	return error;
}

void system_local_user_array_free(UT_array **users)
{
	utarray_free(*users);
	*users = NULL;
}

static void system_local_user_copy_fn(void *dst, const void *src)
{
	system_local_user_t *d = (system_local_user_t *) dst;
	system_local_user_t *s = (system_local_user_t *) src;
	system_authorized_key_t *key = NULL;

	system_local_user_init(d);

	system_local_user_set_name(d, s->name);
	system_local_user_set_password(d, s->password);
	system_local_user_set_nologin(d, s->nologin);

	while ((key = utarray_next(s->keys, key)) != NULL) {
		utarray_push_back(d->keys, key);
	}
}

static void system_local_user_dtor_fn(void *elt)
{
	system_local_user_free(elt);
}