#include "local_user.h"

#include "authorized_key/array.h"
#include "utils/memory.h"

void system_local_user_init(system_local_user_t *user)
{
	*user = (system_local_user_t){0};
}

int system_local_user_set_name(system_local_user_t *user, const char *name)
{
	if (user->name) {
		free(user->name);
		user->name = 0;
	}

	user->name = xstrdup(name);

	return user->name != NULL;
}

int system_local_user_set_password(system_local_user_t *user, const char *password)
{
	if (user->password) {
		free(user->password);
		user->password = 0;
	}

	user->password = xstrdup(password);

	return user->password != NULL;
}

int system_local_user_set_nologin(system_local_user_t *user, const int nologin)
{
	user->nologin = nologin;
	return 0;
}

void system_local_user_free(system_local_user_t *user)
{
	if (user->name) {
		free(user->name);
	}

	if (user->password) {
		free(user->password);
	}
}

int system_local_user_cmp_fn(const void *e1, const void *e2)
{
	system_local_user_t *u1 = (system_local_user_t *) e1;
	system_local_user_t *u2 = (system_local_user_t *) e2;

	return strcmp(u1->name, u2->name);
}