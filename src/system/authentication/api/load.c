#include "load.h"
#include "types.h"

#include "system/authentication/data/local_user/array.h"
#include "system/authentication/data/local_user.h"

#include <unistd.h>
#include <pwd.h>
#include <shadow.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <linux/limits.h>

int system_authentication_load_user(system_ctx_t *ctx, UT_array **arr)
{
	int error = 0;

	struct passwd *pwd = NULL;
	struct spwd *pwdshd = NULL;

	system_local_user_t tmp_user = {0};
	system_local_user_t *user_iter = NULL;
	char dir_buffer[PATH_MAX] = {0};

	// adding username
	while ((pwd = getpwent()) != NULL) {
		if ((pwd->pw_uid >= 1000 && strncmp(pwd->pw_dir, "/home", sizeof("/home") - 1) == 0) || (pwd->pw_uid == 0)) {
			tmp_user.name = pwd->pw_name;

			error = system_local_user_array_add(arr, tmp_user);
			if (error != 0) {
				goto error_out;
			}
		}
	}

	// adding password
	while ((pwdshd = getspent()) != NULL) {
		tmp_user.name = pwdshd->sp_namp;

		system_local_user_t *found = utarray_find(*arr, &tmp_user, system_local_user_cmp_fn);
		if (found) {
			/* A password field which starts with a exclamation mark means that
			 * the password is locked.
			 * Majority of root accounts are paswordless and therefore contain
			 * only asterisk in the password field.
			 * Set the value to NULL in both cases.
			 */
			if (strcmp(pwdshd->sp_pwdp, "!") != 0 && strcmp(pwdshd->sp_pwdp, "*") != 0) {
				system_local_user_set_password(found, pwdshd->sp_pwdp);
			}
		}
	}

	while ((user_iter = utarray_next(*arr, user_iter)) != NULL) {
		if (!strncmp(user_iter->name, "root", 4)) {
			if (snprintf(dir_buffer, sizeof(dir_buffer), "/root/.ssh") < 0) {
				goto error_out;
			}
		} else {
			if (snprintf(dir_buffer, sizeof(dir_buffer), "/home/%s/.ssh", user_iter->name) < 0) {
				goto error_out;
			}
		}
		// error = local_user_get_key_info(user_iter, dir_buffer);
		// if (error == 1) {
		// 	/* .ssh dir doesn't exist for this user */
		// 	continue;
		// } else if (error != 0) {
		// 	goto fail;
		// }
	}

	goto out;

error_out:
	error = -1;

out:
	endpwent();

	return error;
}