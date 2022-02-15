/*
 * telekom / sysrepo-plugin-system
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2021 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "user_authentication.h"
#include "utils/memory.h"
#include "utils/uthash/utarray.h"
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pwd.h>
#include <shadow.h>
#include <dirent.h>

// uthash local_user
void local_user_copy_fn(void *dst, const void *src);
void local_user_dtor_fn(void *elt);
int local_user_cmp_fn(const void *p1, const void *p2);

// uthash authorized_key
void authorized_key_copy_fn(void *dst, const void *src);
void authorized_key_dtor_fn(void *elt);
int authorized_key_cmp_fn(const void *p1, const void *p2);

// helpers

int get_existing_users_from_passwd(char *existing_users[], int *num_users);

int remove_user_entry(char *name);
int remove_line_from_file(char *orig, char *tmp, char *backup, char *username);
int remove_ssh_file(char *username, char *filename);
int remove_home_dir(char *username);

int create_dir(char *dir_path, char *username);
int writing_to_key_file(char *in_dir, char *key_name, char *key_algorithm, char *key_data);

// int add_pub_extension(char *name);
void remove_file_name_extension(char *name);
void edit_path(char *new_path, char *old_path, char *name);

int set_passwd_file(char *username);
int set_shadow_file(char *username, char *password);
int copy_file(char *src, char *dst);
int create_home_dir(char *username, char *home_dir_buffer, size_t buffer_size);
int create_ssh_dir(char *username, const char *home_dir_buffer, char *ssh_dir_buffer, size_t buffer_size);
int set_owner(char *path);

void authorized_key_init(authorized_key_t *key)
{
	key->name = NULL;
	key->algorithm = NULL;
	key->key_data = NULL;
}

int authorized_key_set_name(authorized_key_t *key, const char *name)
{
	int error = 0;

	if (key->name) {
		FREE_SAFE(key->name);
	}

	key->name = name ? xstrdup(name) : NULL;

	return error;
}

int authorized_key_set_algorithm(authorized_key_t *key, const char *algorithm)
{
	int error = 0;

	if (key->algorithm) {
		FREE_SAFE(key->algorithm);
	}

	key->algorithm = algorithm ? xstrdup(algorithm) : NULL;

	return error;
}

int authorized_key_set_key_data(authorized_key_t *key, const char *key_data)
{
	int error = 0;

	if (key->key_data) {
		FREE_SAFE(key->key_data);
	}

	key->key_data = key_data ? xstrdup(key_data) : NULL;

	return error;
}

void authorized_key_free(authorized_key_t *key)
{
	if (key->name) {
		FREE_SAFE(key->name);
	}

	if (key->algorithm) {
		FREE_SAFE(key->algorithm);
	}

	if (key->key_data) {
		FREE_SAFE(key->key_data);
	}
}

void authorized_key_array_init(UT_array **keys)
{
	UT_icd auth_icd = {
		.sz = sizeof(authorized_key_t),
		.copy = authorized_key_copy_fn,
		.dtor = authorized_key_dtor_fn,
		.init = NULL,
	};
	utarray_new(*keys, &auth_icd);
	utarray_reserve(*keys, MAX_AUTH_KEYS);
}

int authorized_key_array_add_key(UT_array **keys, const char *key)
{
	int error = 0;
	authorized_key_t *found = NULL;
	authorized_key_t tmp_key = {.name = (char *) key, NULL};

	if (utarray_len(*keys) >= MAX_AUTH_KEYS) {
		return -1;
	}

	found = utarray_find(*keys, &tmp_key, authorized_key_cmp_fn);
	if (!found) {
		// add the key + sort array
		utarray_push_back(*keys, &tmp_key);
		utarray_sort(*keys, authorized_key_cmp_fn);
	} else {
		// key already exists -> error
		error = -1;
	}

	return error;
}

int authorized_key_array_set_algorithm(UT_array **keys, const char *key, const char *algorithm)
{
	int error = 0;
	authorized_key_t *found = NULL;
	authorized_key_t tmp_key = {.name = (char *) key, NULL};

	found = utarray_find(*keys, &tmp_key, authorized_key_cmp_fn);
	if (found) {
		// set key algorithm
		error = authorized_key_set_algorithm(found, algorithm);
	} else {
		// unknown key -> error
		error = -1;
	}

	return error;
}

int authorized_key_array_set_key_data(UT_array **keys, const char *key, const char *algorithm)
{
	int error = 0;
	authorized_key_t *found = NULL;
	authorized_key_t tmp_key = {.name = (char *) key, NULL};

	found = utarray_find(*keys, &tmp_key, authorized_key_cmp_fn);
	if (found) {
		// set key data
		error = authorized_key_set_key_data(found, algorithm);
	} else {
		// unknown key -> error
		error = -1;
	}

	return error;
}

int authorized_key_array_set_ssh_key(UT_array **keys, const char *dir)
{
	int error = 0;

	char *ssh_filename = NULL;
	char *ssh_key_algo = NULL;
	char *ssh_key_data = NULL;

	authorized_key_t *key_iter = NULL;

	while ((key_iter = utarray_next(*keys, key_iter)) != NULL) {
		ssh_filename = key_iter->name;
		ssh_key_algo = key_iter->algorithm;
		ssh_key_data = key_iter->key_data;

		error = writing_to_key_file((char *) dir, ssh_filename, ssh_key_algo, ssh_key_data);
		if (error != 0) {
			goto error_out;
		}
	}

	return 0;

error_out:

	return -1;
}

void authorized_key_array_free(UT_array **keys)
{
	utarray_free(*keys);
	*keys = NULL;
}

void local_user_init(local_user_t *user)
{
	user->name = NULL;
	user->password = NULL;
	user->nologin = false;
	authorized_key_array_init(&user->auth_keys);
}

int local_user_set_name(local_user_t *user, const char *name)
{
	if (user->name) {
		FREE_SAFE(user->name);
	}

	user->name = name ? xstrdup(name) : NULL;

	return 0;
}

int local_user_set_password(local_user_t *user, const char *password)
{
	if (user->password) {
		FREE_SAFE(user->password);
	}

	user->password = password ? xstrdup(password) : NULL;

	return 0;
}

int local_user_set_nologin(local_user_t *user, bool nologin)
{
	user->nologin = nologin;
	return 0;
}

int local_user_get_key_info(local_user_t *user, const char *dir)
{
	int error = 0;
	FILE *entry_file = NULL;
	DIR *FD = NULL;
	struct dirent *in_file = NULL;

	char fpath_buffer[PATH_MAX] = {0};
	char alg_buffer[MAX_ALG_SIZE] = {0};
	char key_data_buffer[MAX_KEY_DATA_SIZE] = {0};

	if ((FD = opendir(dir)) == NULL) {
		error = 1;
		goto out;
	} else {
		while ((in_file = readdir(FD))) {
			char key_name[100] = {0};

			if (!has_pub_extension(in_file->d_name)) {
				continue;
			}

			if (snprintf(fpath_buffer, sizeof(fpath_buffer), "%s/%s", dir, in_file->d_name) < 0) {
				error = -1;
				goto out;
			}

			entry_file = fopen(fpath_buffer, "r");
			if (entry_file == NULL) {
				error = 1;
				goto out;
			}

			snprintf(key_name, sizeof(key_name), "%s", in_file->d_name);
			error = authorized_key_array_add_key(&user->auth_keys, key_name);
			if (error) {
				goto out;
			}

			rewind(entry_file);
			fscanf(entry_file, "%s %s", alg_buffer, key_data_buffer);

			error = authorized_key_array_set_algorithm(&user->auth_keys, key_name, alg_buffer);
			if (error != 0) {
				goto out;
			}

			error = authorized_key_array_set_key_data(&user->auth_keys, key_name, key_data_buffer);
			if (error != 0) {
				goto out;
			}

			fclose(entry_file);
			entry_file = NULL;
		}
	}

out:
	if (entry_file != NULL) {
		fclose(entry_file);
	}

	closedir(FD);
	return error;
}

void local_user_free(local_user_t *user)
{
	if (user->name) {
		FREE_SAFE(user->name);
	}

	if (user->password) {
		FREE_SAFE(user->password);
	}

	authorized_key_array_free(&user->auth_keys);
}

/* TODO:
 * 		- add entry to /etc/group
 */
bool has_pub_extension(char *name)
{
	size_t len = strlen(name);
	return len > 4 && strcmp(name + len - 4, ".pub") == 0;
}

int create_home_dir(char *username, char *home_dir_buffer, size_t buffer_size)
{
	int error = 0;
	DIR *home_dir = NULL;

	// create home dir path
	if (strcmp(username, ROOT_USERNAME) == 0) {
		// root
		if (snprintf(home_dir_buffer, buffer_size, "/root/") < 0) {
			goto error_out;
		}
	} else {
		// regular users
		if (snprintf(home_dir_buffer, buffer_size, "/home/%s", username) < 0) {
			goto error_out;
		}
	}

	// check if home dir exists
	home_dir = opendir(home_dir_buffer);
	if (home_dir != NULL) {
		// dir exists
	} else if (ENOENT == errno) {
		// dir doesn't exist
		// create home dir
		error = create_dir(home_dir_buffer, username);
		if (error != 0) {
			goto error_out;
		}
	} else {
		// opendir failed
		goto error_out;
	}

	closedir(home_dir);
	return 0;

error_out:
	if (home_dir != NULL) {
		closedir(home_dir);
	}
	return -1;
}

int create_ssh_dir(char *username, const char *home_dir_buffer, char *ssh_dir_buffer, size_t buffer_size)
{
	int error = 0;
	DIR *ssh_dir = NULL;

	// create .ssh dir path
	if (snprintf(ssh_dir_buffer, buffer_size, "%s/.ssh", home_dir_buffer) < 0) {
		goto error_out;
	}

	// check if .ssh dir exists
	ssh_dir = opendir(ssh_dir_buffer);
	if (ssh_dir != NULL) {
		// dir exists
	} else if (ENOENT == errno) {
		// dir doesn't exist
		// create home dir
		error = create_dir(ssh_dir_buffer, username);
		if (error != 0) {
			goto error_out;
		}
	} else {
		// opendir failed
		goto error_out;
	}

	closedir(ssh_dir);
	return 0;

error_out:
	if (ssh_dir != NULL) {
		closedir(ssh_dir);
	}
	return -1;
}

int remove_user_entry(char *name)
{
	int error = 0;

	// remove entry in passwd file
	error = remove_line_from_file(PASSWD_FILE, USER_TEMP_PASSWD_FILE, PASSWD_BAK_FILE, name);
	if (error != 0) {
		return -1;
	}

	// remove entry in shadow file
	error = remove_line_from_file(SHADOW_FILE, USER_TEMP_SHADOW_FILE, SHADOW_BAK_FILE, name);
	if (error != 0) {
		return -1;
	}

	return 0;
}

int remove_home_dir(char *username)
{
	int error = 0;
	char cmd[PATH_MAX] = {0};
	size_t username_len = 0;
	size_t path_len = 0;

	username_len = strnlen(username, MAX_USERNAME_LEN) + 1;

	// check if username is not root
	if (strncmp(username, "root", username_len) != 0) {
		// non-root user
		path_len = username_len + strlen("rm -rf /home/%s") + 1; // TODO: remove "rm -rf" when nftw() is used

		error = snprintf(cmd, path_len, "rm -rf /home/%s", username);
		if (error < 0) {
			goto error_out;
		}
	}

	// remove the home dir
	// TODO: implement recursive deletion of files and directories in home dir
	//		 use nftw() instead of system
	error = system(cmd);
	if (error != 0) {
		goto error_out;
	}

	return 0;

error_out:
	return -1;
}

int remove_line_from_file(char *orig, char *tmp, char *backup, char *username)
{
	FILE *fp = NULL;
	FILE *fp_tmp = NULL;
	char *line = NULL;
	size_t len = 0;
	ssize_t read = 0;

	fp = fopen(orig, "r");
	if (fp == NULL) {
		goto error_out;
	}

	fp_tmp = fopen(tmp, "w");
	if (fp_tmp == NULL) {
		goto error_out;
	}

	while ((read = getline(&line, &len, fp)) != -1) {
		// skip line that start with name
		if (strncmp(line, username, strnlen(username, MAX_USERNAME_LEN)) != 0) {
			// copy line to temp file
			fputs(line, fp_tmp);
		}
	}

	FREE_SAFE(line);
	fclose(fp);
	fclose(fp_tmp);

	// create a backup file
	if (copy_file(orig, backup) != 0) {
		goto error_out;
	}

	// copy the temp file to the real file
	if (copy_file(tmp, orig) != 0) {
		goto error_out;
	}

	// remove the temp file
	if (remove(tmp) != 0) {
		goto error_out;
	}

	return 0;

error_out:
	if (fp != NULL) {
		fclose(fp);
	}

	if (fp_tmp != NULL) {
		fclose(fp_tmp);
	}

	if (line != NULL) {
		FREE_SAFE(line);
	}

	if (access(orig, F_OK) != 0) {
		rename(backup, orig);
	}

	return -1;
}

int remove_ssh_file(char *username, char *filename)
{
	int error = 0;
	char file_path[PATH_MAX] = {0};
	size_t username_len = 0;
	size_t filename_len = 0;
	size_t file_path_len = 0;

	username_len = strnlen(username, MAX_USERNAME_LEN);
	filename_len = strnlen(filename, NAME_MAX);

	// check if username is root, .ssh dir of root is located separate from users .ssh dir
	if (strncmp(username, "root", username_len) == 0) {
		file_path_len = username_len + filename_len + strlen("/root/.ssh/%s") + 1;

		error = snprintf(file_path, file_path_len, "/root/.ssh/%s", filename);
		if (error < 0) {
			goto error_out;
		}
	} else {
		// non-root user
		file_path_len = username_len + filename_len + strlen("/home/%s/.ssh/%s") + 1;

		error = snprintf(file_path, file_path_len, "/home/%s/.ssh/%s", username, filename);
		if (error < 0) {
			goto error_out;
		}
	}

	// remove the file
	error = remove(file_path);
	if (error != 0) {
		goto error_out;
	}

	return 0;

error_out:
	return -1;
}

int get_existing_users_from_passwd(char *existing_users[], int *num_users)
{
	struct passwd *pwd = {0};
	int count = 1; // counter starts at 1 because we manually added "root" first

	// root will always be present, so set it as first element
	existing_users[0] = xstrdup("root");

	setpwent();

	pwd = getpwent();
	if (pwd == NULL) {
		goto error_out;
	}

	do {
		// check if user has uid => 1000 and iss < 65534
		// uid 65534 belongs to the "nobody" user
		if (pwd->pw_uid >= 1000 && pwd->pw_uid < 65534) {
			existing_users[count] = xstrdup(pwd->pw_name);
			count++;
		}
	} while ((pwd = getpwent()) != NULL);

	endpwent();

	*num_users = count;

	return 0;

error_out:
	return -1;
}

int writing_to_key_file(char *in_dir, char *key_name, char *key_algorithm, char *key_data)
{
	size_t string_len = 0;
	FILE *file_key = NULL;
	char *file_path = NULL;

	string_len = strlen(in_dir) + strlen("/") + strlen(key_name) + 1;
	file_path = xmalloc(string_len);

	snprintf(file_path, string_len, "%s/%s", in_dir, key_name);

	file_key = fopen(file_path, "w");
	if (file_key == NULL) {
		fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
		FREE_SAFE(file_path);
		return -1;
	}

	fprintf(file_key, "%s\n", key_algorithm);
	fprintf(file_key, "%s", key_data);

	if (set_owner(file_path) != 0) {
		FREE_SAFE(file_path);
		return -1;
	}

	if (chmod(in_dir, 0700) != 0) {
		FREE_SAFE(file_path);
		return -1;
	}

	fclose(file_key);
	FREE_SAFE(file_path);

	return 0;
}

int set_owner(char *path)
{
	struct passwd *pwd = NULL;
	setpwent();

	pwd = getpwent();
	if (pwd == NULL) {
		return -1;
	}

	do {
		// check if username in dir path
		if (strstr(pwd->pw_name, path) != 0) {
			// set ownership of file
			if (chown(path, pwd->pw_uid, pwd->pw_gid)) {
				return -1;
			}
			break;
		}
	} while ((pwd = getpwent()) != NULL);

	endpwent();
	return 0;
}

int create_dir(char *dir_path, char *username)
{
	int error = 0;
	struct passwd *pwd = {0};

	error = mkdir(dir_path, 0700);
	if (error != 0) {
		return -1;
	}

	setpwent();

	pwd = getpwent();
	if (pwd == NULL) {
		return -1;
	}

	do {
		if (strcmp(username, pwd->pw_name) == 0) {
			// set permissions to dir
			if (chown(dir_path, pwd->pw_uid, pwd->pw_gid)) {
				return -1;
			}
			break;
		}
	} while ((pwd = getpwent()) != NULL);

	endpwent();

	return 0;
}

int set_passwd_file(char *username)
{
	struct passwd *pwd = {0};
	struct passwd p = {0};
	size_t username_len = 0;
	FILE *tmp_pwf = NULL; // temporary passwd file
	uid_t last_uid = 0;
	gid_t last_gid = 0;

	tmp_pwf = fopen(USER_TEMP_PASSWD_FILE, "w");
	if (!tmp_pwf) {
		goto error_out;
	}

	setpwent();

	pwd = getpwent();
	if (pwd == NULL) {
		goto error_out;
	}

	do {
		// copy passwd file to temp file
		if (putpwent(pwd, tmp_pwf) != 0) {
			goto error_out;
		}

		if (pwd->pw_uid >= 1000) {
			last_uid = pwd->pw_uid;
			last_gid = pwd->pw_gid;
		}

	} while ((pwd = getpwent()) != NULL);

	// add the new users entry into the temp file
	p.pw_name = xstrdup(username);
	p.pw_passwd = strdup("x");
	p.pw_shell = strdup("/bin/bash");

	p.pw_uid = ++last_uid;
	p.pw_gid = ++last_gid;

	username_len = strlen("/home/") + strlen(p.pw_name) + 1;
	p.pw_dir = xmalloc(username_len);
	if (snprintf(p.pw_dir, username_len, "/home/%s", p.pw_name) < 0) {
		goto error_out;
	}

	if (putpwent(&p, tmp_pwf) != 0) {
		goto error_out;
	}

	FREE_SAFE(p.pw_name);
	FREE_SAFE(p.pw_passwd);
	FREE_SAFE(p.pw_shell);
	FREE_SAFE(p.pw_dir);

	endpwent();

	fclose(tmp_pwf);
	tmp_pwf = NULL;

	// create a backup file of /etc/passwd
	if (rename(PASSWD_FILE, PASSWD_BAK_FILE) != 0)
		goto error_out;

	// copy the temp file to /etc/passwd
	if (copy_file(USER_TEMP_PASSWD_FILE, PASSWD_FILE) != 0) {
		goto error_out;
	}

	// remove the temp file
	if (remove(USER_TEMP_PASSWD_FILE) != 0)
		goto error_out;

	return 0;

error_out:
	if (tmp_pwf != NULL) {
		fclose(tmp_pwf);
	}

	if (p.pw_name != NULL) {
		FREE_SAFE(p.pw_name);
	}

	if (p.pw_passwd != NULL) {
		FREE_SAFE(p.pw_passwd);
	}

	if (p.pw_shell != NULL) {
		FREE_SAFE(p.pw_shell);
	}

	if (p.pw_dir != NULL) {
		FREE_SAFE(p.pw_dir);
	}

	return -1;
}

int set_shadow_file(char *username, char *password)
{
	struct spwd *shd = {0};
	struct spwd s = {0};
	FILE *tmp_shf = NULL;
	size_t username_len = 0;

	tmp_shf = fopen(USER_TEMP_SHADOW_FILE, "w");
	if (!tmp_shf) {
		goto error_out;
	}

	// copy existing shadow entries to temp file
	setspent();

	shd = getspent();
	if (shd == NULL) {
		goto error_out;
	}

	do {
		if (putspent(shd, tmp_shf) != 0) {
			goto error_out;
		}

	} while ((shd = getspent()) != NULL);

	// set new users shadow entry
	username_len = strlen(username) + 1;
	s.sp_namp = strndup(username, username_len);

	username_len = strlen(password) + 1;
	s.sp_pwdp = strndup(password, username_len);
	s.sp_lstchg = -1; // -1 value corresponds to an empty string
	s.sp_max = 99999;
	s.sp_min = 0;
	s.sp_warn = 7;

	s.sp_expire = -1;
	//s.sp_flag = 0; // not used
	s.sp_inact = -1;

	if (putspent(&s, tmp_shf) != 0) {
		goto error_out;
	}

	FREE_SAFE(s.sp_namp);
	FREE_SAFE(s.sp_pwdp);

	endspent();
	fclose(tmp_shf);
	tmp_shf = NULL;

	// create a backup file of /etc/shadow
	if (copy_file(SHADOW_FILE, SHADOW_BAK_FILE) != 0) {
		printf("copy_file error: %s", strerror(errno));
		goto error_out;
	}

	// copy the temp file to /etc/shadow
	if (copy_file(USER_TEMP_SHADOW_FILE, SHADOW_FILE) != 0) {
		printf("copy_file error: %s", strerror(errno));
		goto error_out;
	}

	// remove the temp file
	if (remove(USER_TEMP_SHADOW_FILE) != 0) {
		goto error_out;
	}

	return 0;

error_out:
	if (s.sp_namp != NULL) {
		FREE_SAFE(s.sp_namp);
	}

	if (s.sp_pwdp != NULL) {
		FREE_SAFE(s.sp_pwdp);
	}

	if (access(PASSWD_FILE, F_OK) != 0)
		rename(PASSWD_BAK_FILE, PASSWD_FILE);

	if (tmp_shf != NULL)
		fclose(tmp_shf);

	return -1;
}

int copy_file(char *src, char *dst)
{
	int read_fd = -1;
	int write_fd = -1;
	struct stat stat_buf = {0};
	off_t offset = 0;

	read_fd = open(src, O_RDONLY);
	if (read_fd == -1) {
		goto error_out;
	}

	if (fstat(read_fd, &stat_buf) != 0) {
		goto error_out;
	}

	write_fd = open(dst, O_CREAT | O_WRONLY | O_TRUNC, stat_buf.st_mode);
	if (write_fd == -1) {
		goto error_out;
	}

	if (sendfile(write_fd, read_fd, &offset, (size_t) stat_buf.st_size) == -1) {
		goto error_out;
	}

	close(read_fd);
	close(write_fd);

	return 0;

error_out:
	if (read_fd != -1) {
		close(read_fd);
	}

	if (write_fd != -1) {
		close(write_fd);
	}

	return -1;
}

int local_user_array_init(UT_array **users)
{
	int error = 0;

	UT_icd users_icd = (UT_icd){
		.sz = sizeof(local_user_t),
		.init = NULL,
		.copy = local_user_copy_fn,
		.dtor = local_user_dtor_fn,
	};

	utarray_new(*users, &users_icd);
	utarray_reserve(*users, MAX_LOCAL_USERS);

	error = local_user_array_add_existing(users);
	if (error) {
		return -1;
	}

	return error;
}

int local_user_array_add_existing(UT_array **users)
{
	int error = 0;

	struct passwd *pwd = NULL;
	struct spwd *pwdshd = NULL;

	local_user_t tmp_user = {0};
	local_user_t *user_iter = NULL;
	char dir_buffer[PATH_MAX] = {0};

	setpwent();

	// adding username
	while ((pwd = getpwent()) != NULL) {
		if ((pwd->pw_uid >= 1000 && strncmp(pwd->pw_dir, HOME_PATH, strlen(HOME_PATH)) == 0) || (pwd->pw_uid == 0)) {
			error = local_user_array_add_user(users, pwd->pw_name);
			if (error != 0) {
				goto fail;
			}
		}
	}

	// enable use of _find()
	utarray_sort(*users, local_user_cmp_fn);

	// adding password
	while ((pwdshd = getspent()) != NULL) {
		tmp_user.name = pwdshd->sp_namp;

		local_user_t *found = utarray_find(*users, &tmp_user, local_user_cmp_fn);
		if (found) {
			error = local_user_set_password(found, pwdshd->sp_pwdp);
			if (error) {
				goto fail;
			}
		}
	}

	while ((user_iter = utarray_next(*users, user_iter)) != NULL) {
		if (!strncmp(user_iter->name, "root", 4)) {
			if (snprintf(dir_buffer, sizeof(dir_buffer), "/root/.ssh") < 0) {
				goto fail;
			}
		} else {
			if (snprintf(dir_buffer, sizeof(dir_buffer), "/home/%s/.ssh", user_iter->name) < 0) {
				goto fail;
			}
		}
		error = local_user_get_key_info(user_iter, dir_buffer);
		if (error != 0) {
			goto fail;
		}
	}

	goto out;

fail:
	error = -1;

out:
	endpwent();

	return -1;
}

int local_user_array_add_user(UT_array **users, const char *name)
{
	int error = 0;
	local_user_t *found = NULL;
	local_user_t user = {0};

	if (utarray_len(*users) >= MAX_LOCAL_USERS) {
		return EINVAL;
	}

	user.name = (char *) name;
	found = utarray_find(*users, &user, local_user_cmp_fn);

	if (!found) {
		// add to the array + sort because of find() functions
		utarray_push_back(*users, &user);
		utarray_sort(*users, local_user_cmp_fn);
	}

	return error;
}

int local_user_array_set_password(UT_array **users, const char *name, const char *password)
{
	int error = 0;
	local_user_t *found = NULL;
	local_user_t user = {.name = (char *) name};

	found = utarray_find(*users, &user, local_user_cmp_fn);

	if (found) {
		error = local_user_set_password(found, password);
	} else {
		error = -1;
	}

	return error;
}

int local_user_array_add_key(UT_array **users, const char *name, const char *key)
{
	int error = 0;

	local_user_t *found = NULL;
	local_user_t user = {.name = (char *) name};

	found = utarray_find(*users, &user, local_user_cmp_fn);

	if (found) {
		error = authorized_key_array_add_key(&found->auth_keys, key);
	} else {
		error = -1;
	}

	return error;
}

int local_user_array_set_key_algorithm(UT_array **users, const char *name, const char *key, const char *algorithm)
{
	int error = 0;

	local_user_t *found = NULL;
	local_user_t user = {.name = (char *) name};

	found = utarray_find(*users, &user, local_user_cmp_fn);

	if (found) {
		error = authorized_key_array_set_algorithm(&found->auth_keys, key, algorithm);
	} else {
		error = -1;
	}

	return error;
}

int local_user_array_set_key_data(UT_array **users, const char *name, const char *key, const char *key_data)
{
	int error = 0;

	local_user_t *found = NULL;
	local_user_t user = {.name = (char *) name};

	found = utarray_find(*users, &user, local_user_cmp_fn);

	if (found) {
		error = authorized_key_array_set_key_data(&found->auth_keys, key, key_data);
	} else {
		error = -1;
	}

	return error;
}

int local_user_array_set_new_users(UT_array **users)
{
	int error = 0;
	char *existing_users[MAX_LOCAL_USERS] = {0};
	int num_existing_users = 0;
	bool user_exists = false;

	char home_dir_buffer[PATH_MAX] = {0};
	char ssh_dir_buffer[PATH_MAX] = {0};

	local_user_t *user_iter = NULL;

	// check if username password or public key algorithm is an empty string
	// remove users or ssh public key files if so
	error = local_user_array_delete_users(users);
	if (error != 0) {
		goto error_out;
	}

	// get list of already existing user names from passwd file
	error = get_existing_users_from_passwd(existing_users, &num_existing_users);
	if (error != 0) {
		goto error_out;
	}

	while ((user_iter = utarray_next(*users, user_iter)) != NULL) {
		user_exists = false;
		for (int j = 0; j < num_existing_users; j++) {
			if (strcmp(user_iter->name, existing_users[j]) == 0) {
				// user is already in passwd
				// skip this user
				user_exists = true;
				break;
			}
		}

		if (!user_exists) { // if the user doesn't already exist in passwd, add the user
			// add this user to passwd
			error = set_passwd_file(user_iter->name);
			if (error != 0) {
				goto error_out;
			}

			// add this user to shadow
			error = set_shadow_file(user_iter->name, user_iter->password);
			if (error != 0) {
				goto error_out;
			}
		}

		// create home dir if it doesn't exist
		error = create_home_dir(user_iter->name, home_dir_buffer, sizeof(home_dir_buffer));
		if (error != 0) {
			goto error_out;
		}

		if (utarray_len(user_iter->auth_keys) > 0) {
			error = create_ssh_dir(user_iter->name, home_dir_buffer, ssh_dir_buffer, sizeof(ssh_dir_buffer));
			if (error != 0) {
				goto error_out;
			}

			// set ssh keys for users
			error = authorized_key_array_set_ssh_key(&user_iter->auth_keys, ssh_dir_buffer);
			if (error != 0) {
				goto error_out;
			}
		}

		// TODO: investigate if necessarry
		memset(home_dir_buffer, 0, sizeof(home_dir_buffer));
		memset(ssh_dir_buffer, 0, sizeof(ssh_dir_buffer));
	}

	goto out;

error_out:
	error = -1;

out:
	// cleanup existing_users
	for (int i = 0; i < num_existing_users; i++) {
		FREE_SAFE(existing_users[i]);
	}

	return error;
}

int local_user_array_delete_users(UT_array **users)
{
	int error = 0;
	bool remove_user = false;
	local_user_t *user_iter = NULL;
	authorized_key_t *key_iter = NULL;
	unsigned int user_count = 0;
	unsigned int key_count = 0;

	while ((user_iter = utarray_next(*users, user_iter)) != NULL) {
		remove_user = false;

		if (strcmp(user_iter->password, "") == 0) {
			// remove the user from passwd and shadow file
			error = remove_user_entry(user_iter->name);
			if (error != 0) {
				goto error_out;
			}

			remove_user = true;
		}

		key_count = 0;
		while ((key_iter = utarray_next(user_iter->auth_keys, key_iter)) != NULL) {
			if (strcmp(key_iter->algorithm, "") == 0) {
				// remove the ssh public key file
				error = remove_ssh_file(user_iter->name, key_iter->name);
				if (error != 0) {
					goto error_out;
				}

				// remove current key from the array
				key_iter = utarray_prev(user_iter->auth_keys, key_iter);
				utarray_erase(user_iter->auth_keys, key_count, 1);
				--key_count;
				continue;
			}
			key_count++;
		}

		if (remove_user == true) {
			// remove home dir as well
			error = remove_home_dir(user_iter->name);
			if (error != 0) {
				goto error_out;
			}

			// remove current user from the array
			user_iter = utarray_prev(*users, user_iter);
			utarray_erase(*users, user_count, 1);
			--user_count;
			continue;
		}

		++user_count;
	}

	return 0;

error_out:
	return -1;
}

void local_user_array_free(UT_array **users)
{
	utarray_free(*users);
	*users = NULL;
}

void local_user_copy_fn(void *dst, const void *src)
{
	local_user_t *d = (local_user_t *) dst;
	local_user_t *s = (local_user_t *) src;
	authorized_key_t *tmp = NULL;

	// setup auth_keys
	local_user_init(d);

	local_user_set_name(d, s->name);
	local_user_set_password(d, s->password);
	local_user_set_nologin(d, s->nologin);

	if (s->auth_keys) {
		while ((tmp = utarray_next(s->auth_keys, tmp)) != NULL) {
			utarray_push_back(d->auth_keys, tmp);
		}
	}
}

void local_user_dtor_fn(void *elt)
{
	local_user_free(elt);
}

int local_user_cmp_fn(const void *p1, const void *p2)
{
	local_user_t *u1 = (local_user_t *) p1;
	local_user_t *u2 = (local_user_t *) p2;
	return strcmp(u1->name, u2->name);
}

void authorized_key_copy_fn(void *dst, const void *src)
{
	authorized_key_t *d = (authorized_key_t *) dst;
	authorized_key_t *s = (authorized_key_t *) src;

	authorized_key_init(d);

	authorized_key_set_name(d, s->name);
	authorized_key_set_algorithm(d, s->algorithm);
	authorized_key_set_key_data(d, s->key_data);
}

void authorized_key_dtor_fn(void *elt)
{
	authorized_key_free(elt);
}

int authorized_key_cmp_fn(const void *p1, const void *p2)
{
	authorized_key_t *k1 = (authorized_key_t *) p1;
	authorized_key_t *k2 = (authorized_key_t *) p2;
	return strcmp(k1->name, k2->name);
}
