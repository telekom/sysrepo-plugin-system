#include "user_authentication.h"
#include "utils/memory.h"
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

/* TODO:
 * 		- add entry to /etc/group
 */

bool has_pub_extension(char *name)
{
	size_t len = strlen (name);
	return len > 4 && strcmp(name + len - 4, ".pub") == 0;
}
/*
int add_pub_extension(char *name)
{
	size_t tmp_len = 0;
	char *temp_string = NULL;

	tmp_len = strlen(name) + strlen(".pub") + 1;
	temp_string = xmalloc(tmp_len);
	if (snprintf(temp_string, tmp_len, "%s.pub", name) < 0) {
		goto fail;
	}

	name = xrealloc(name, tmp_len);
	if (snprintf(name, tmp_len, "%s", temp_string) < 0) {
		goto fail;
	}
	FREE_SAFE(temp_string);
	return 0;
fail:
	FREE_SAFE(temp_string);
	return -1;
} */
/*
void remove_file_name_extension(char *name)
{
	size_t len = strlen (name);
	strncpy (name, name, len - 4);
} 
*/

int set_new_users(local_user_list_t *ul)
{
	int error = 0;
	char *existing_users[MAX_LOCAL_USERS] = {0};
	int num_existing_users = 0;
	size_t username_len = 0;
	bool user_exists = false;
	char *user_home_dir_path = NULL;
	char *user_ssh_dir_path = NULL;

	// check if username password or public key algorithm is an empty string
	// remove users or ssh public key files if so
	error = delete_users(ul);
	if (error != 0) {
		goto error_out;
	}

	// get list of already existing user names from passwd file
	error = get_existing_users_from_passwd(existing_users, &num_existing_users);

	// iterate through internal user list
	for (int i = 0; i < MAX_LOCAL_USERS; i++) {
		if (ul->users[i].name == NULL) {
			continue; // in case a user was deleted somewhere in the internal list
		}

		user_exists = false;

		// iterate through existing users in passwd file
		for (int j = 0; j < num_existing_users; j++) {
			username_len = strnlen(existing_users[j], MAX_USERNAME_LEN);

			// check if current user from internal list is already in passwd file
			if (strncmp(ul->users[i].name, existing_users[j], username_len) == 0 ) {
				// user is already in passwd
				// skip this user
				user_exists = true;
				break;
			}
		}

		if (!user_exists) { // if the user doesn't already exist in passwd, add the user
			// add this user to passwd
			error = set_passwd_file(ul->users[i].name);
			if (error != 0) {
				goto error_out;
			}

			// add this user to shadow
			error = set_shadow_file(ul->users[i].name, ul->users[i].password);
			if (error != 0) {
				goto error_out;
			}
		}

		// create home dir if it doesn't exist
		error = create_home_dir(ul->users[i].name, &user_home_dir_path);
		if (error != 0) {
			goto error_out;
		}

		if (ul->users[i].auth.count > 0) {
			// create ssh dir, if it doesn't exist
			error = create_ssh_dir(ul->users[i].name, user_home_dir_path, &user_ssh_dir_path);
			if (error != 0) {
				goto error_out;
			}

			// set ssh keys for users
			error = set_ssh_key(&ul->users[i].auth, user_ssh_dir_path);
			if (error != 0) {
				goto error_out;
			}

			FREE_SAFE(user_ssh_dir_path);
		}

		FREE_SAFE(user_home_dir_path);

	}

	// cleanup existing_users
	for (int i = 0; i < num_existing_users; i++) {
		FREE_SAFE(existing_users[i]);
	}

	return 0;

error_out:
	if (num_existing_users > 0) {
		for (int i = 0; i < num_existing_users; i++) {
			FREE_SAFE(existing_users[i]);
		}
	}

	if (user_home_dir_path != NULL) {
		FREE_SAFE(user_home_dir_path);
	}

	if (user_ssh_dir_path != NULL) {
		FREE_SAFE(user_ssh_dir_path);
	}

	return -1;
}

int create_home_dir(char *username, char **home_dir_path)
{
	int error = 0;
	size_t len = 0;
	DIR *home_dir = NULL;

	// create home dir path
	if (strcmp(username, ROOT_USERNAME) == 0) {
		len = strlen("/root/") + 1;
		*home_dir_path = xstrndup("/root/", len);
	} else {
		// regular users
		len = strlen("/home/") + strlen(username) + 1;
		*home_dir_path = xmalloc(len);
		if (snprintf(*home_dir_path, len, "/home/%s", username) < 0) {
			goto error_out;
		}
	}

	// check if home dir exists
	home_dir = opendir(*home_dir_path);
	if (home_dir != NULL) {
		// dir exists
	} else if (ENOENT == errno) {
		// dir doesn't exist
		// create home dir
		error = create_dir(*home_dir_path, username);
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
	if (home_dir != NULL){
		closedir(home_dir);
	}
	return -1;
}

int create_ssh_dir(char *username, char *home_dir_path, char **ssh_dir_path)
{
	int error = 0;
	size_t len = 0;
	DIR *ssh_dir = NULL;

	// create .ssh dir path
	len = strlen(home_dir_path) + strlen("/.ssh") + 1;
	*ssh_dir_path = xmalloc(len);
	if (snprintf(*ssh_dir_path, len, "%s/.ssh", home_dir_path) < 0) {
		goto error_out;
	}

	// check if .ssh dir exists
	ssh_dir = opendir(*ssh_dir_path);
	if (ssh_dir != NULL) {
		// dir exists
	} else if (ENOENT == errno) {
		// dir doesn't exist
		// create home dir
		error = create_dir(*ssh_dir_path, username);
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



int delete_users(local_user_list_t *ul)
{
	int error = 0;
	bool remove_user = false;
	bool remove_file = false;

	for (int i = 0; i < ul->count; i++) {
		remove_user = false;

		if (ul->users[i].name == NULL || strlen(ul->users[i].name ) == 0) {
			continue;
		}

		if (strcmp(ul->users[i].password, "") == 0) {
			// remove the user from passwd and shadow file
			error = remove_user_entry(ul->users[i].name);
			if (error != 0) {
				goto error_out;
			}

			remove_user = true;
		}

		for (int j = 0; j < ul->users[i].auth.count; j++) {
			remove_file = false;
			if (ul->users[i].auth.authorized_keys[j].algorithm == NULL) {
				continue;
			}

			if (strcmp(ul->users[i].auth.authorized_keys[j].algorithm, "") == 0) {
				// remove the ssh public key file
				error = remove_ssh_file(ul->users[i].name, ul->users[i].auth.authorized_keys[j].name);
				if (error != 0) {
					goto error_out;
				}

				remove_file = true;
			}

			if (remove_file == true) {
				// remove the ssh file info from internal list
				authorized_key_list_free(&ul->users[i].auth);

				// decrease the counter
				ul->users[i].auth.count--;
			}
		}

		if (remove_user == true) {
			// remove home dir as well
			error = remove_home_dir(ul->users[i].name);
			if (error != 0) {
				goto error_out;
			}

			// remove user from internal list
			local_user_free(&ul->users[i]);

			// decrease the counter
			ul->count--;
		}
	}

	return 0;

error_out:
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

	if (access(orig, F_OK) != 0 ) {
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
	size_t len = 0;

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
			len = strnlen(pwd->pw_name, MAX_USERNAME_LEN);
			existing_users[count] = xstrndup(pwd->pw_name, len);

			count++;
		}
	} while ((pwd = getpwent()) != NULL);

	endpwent();

	*num_users = count;

	return 0;

error_out:
	return -1;
}

int writing_to_key_file(char* in_dir, char *key_name, char *key_algorithm, char *key_data) 
{
	size_t string_len = 0;
	FILE *file_key = NULL;
	char* file_path = NULL;

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

int set_ssh_key(authorized_key_list_t *user_auth, char *ssh_dir_path)
{
	int error = 0;
	char *username = NULL;
	char *ssh_filename = NULL;
	char *ssh_key_algo = NULL;
	char *ssh_key_data = NULL;

	for(int i = 0; i < user_auth->count; i++) {
		username = user_auth->authorized_keys[i].name;

		if (username== NULL) {
			continue;
		}

		ssh_filename = user_auth->authorized_keys[i].name;
		ssh_key_algo = user_auth->authorized_keys[i].algorithm;
		ssh_key_data = user_auth->authorized_keys[i].key_data;

		error = writing_to_key_file(ssh_dir_path, ssh_filename, ssh_key_algo, ssh_key_data);
		if (error != 0) {
			goto error_out;
		}
	}

	return 0;

error_out:

	return -1;
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
	p.pw_shell = strdup ("/bin/bash");

	p.pw_uid = ++last_uid;
	p.pw_gid = ++last_gid;

	username_len = strlen ("/home/") + strlen(p.pw_name) + 1;
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

	username_len = strlen (password) + 1;
	s.sp_pwdp = strndup(password, username_len);
	s.sp_lstchg = -1; // -1 value corresponds to an empty string
	s.sp_max = 99999;
	s.sp_min = 0;
	s.sp_warn = 7;

	s.sp_expire = -1;
	//s.sp_flag = 0; // not used
	s.sp_inact =  -1;

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

	if (access(PASSWD_FILE, F_OK) != 0 )
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

	write_fd = open(dst, O_CREAT|O_WRONLY|O_TRUNC, stat_buf.st_mode);
	if (write_fd == -1) {
		goto error_out;
	}

	if (sendfile(write_fd, read_fd, &offset, (size_t)stat_buf.st_size) == -1) {
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

void authorized_key_init(authorized_key_t *k)
{	
	k->name = NULL;
	k->algorithm = NULL;
	k->key_data = NULL;
}

void authorized_key_free(authorized_key_t *k)
{
	if (k->name) {
		FREE_SAFE(k->name);
	}
	if (k->algorithm) {
		FREE_SAFE(k->algorithm);
	}
	if (k->key_data) {
		FREE_SAFE(k->key_data);
	}
}

void authorized_key_list_init(authorized_key_list_t *ul)
{
	for (int i = 0; i < MAX_AUTH_KEYS; i++) {
		authorized_key_init(&ul->authorized_keys[i]);
	}
	ul->count = 0;
}

void authorized_key_list_free(authorized_key_list_t *ul)
{
	for (int i = 0; i < ul->count; i++) {
		authorized_key_free(&ul->authorized_keys[i]);
	}
}
	
void local_user_init(local_user_t *u)
{
	u->name = NULL;
	u->password = NULL;
	u->nologin = false;

	authorized_key_list_init(&u->auth);	
}

void local_user_free(local_user_t *u)
{
	if (u->name) {
		FREE_SAFE(u->name);
	}
	if (u->password) {
		FREE_SAFE(u->password);
	}

	authorized_key_list_free(&u->auth);	
}

int local_user_add_user(local_user_list_t *ul, char *name)
{
	bool name_found = false;

	if (ul->count >= MAX_LOCAL_USERS) {
		return EINVAL;
	}

	for (int i = 0; i < ul->count; i++) {
		if (ul->users[i].name != NULL) { // in case we deleted a user it will be NULL
			if (strcmp(ul->users[i].name, name) == 0) {
				name_found = true;
				break;
			}
		}
	}

	if (!name_found) {
		// set the new user to the first free one in the list
		// the one with name == 0
		int pos = ul->count;
		for (int i = 0; i < ul->count; i++) {
			if (ul->users[i].name == NULL) {
				pos = i;
				break;
			}
		}

		ul->users[pos].name = xstrdup(name);

		if (pos == ul->count) {
			++ul->count;
		}
	}

	return 0;
}

int local_user_set_password(local_user_list_t *ul, char *name, char *password)
{
	bool local_user_found = false;

	for (int i = 0; i < ul->count; i++ ) {
		if (strcmp(ul->users[i].name, name) == 0) {
			// if password was already set, free it first, and then reset it
			if (ul->users[i].password != NULL) {
				FREE_SAFE(ul->users[i].password);
			}

			size_t tmp_len = 0;
			tmp_len = strlen(password);
			ul->users[i].password = strndup(password, tmp_len + 1);

			if (strcmp(password, "!") == 0 || strcmp(password, "*") == 0) {
				ul->users[i].nologin = true;
			}


			local_user_found = true;
			break;
		}	
	}
	if(!local_user_found) {
		return -1;
	}
	return 0;
}

int local_user_add_key(local_user_list_t *ul, char *name, char *key_name)
{
	bool local_user_found = false;
	size_t tmp_len = 0;

	for (int i = 0; i < ul->count; i++) {
		if (strcmp(ul->users[i].name, name) == 0) {
			local_user_found = true;
	
			for (int j = 0; j < ul->users[i].auth.count; j++) {
				if (strncmp(ul->users[i].auth.authorized_keys[j].name, key_name, strlen(key_name)+1) == 0) {
					return 0;
				}
			}
			tmp_len = strlen(key_name);
			ul->users[i].auth.authorized_keys[ul->users[i].auth.count].name = strndup(key_name, tmp_len + 1);
			ul->users[i].auth.count++;
			break;	
		}		
	}
	if(!local_user_found) {
		return -1;
	}

	return 0;
}

int add_algorithm_key_data(local_user_list_t *ul, char *name, char *key_name, char *data_alg, int flag)
{
	bool local_user_found = false;
	bool key_found = false;

	for (int i = 0; i < ul->count; i++) {
		if (strcmp(ul->users[i].name, name) == 0) {
			local_user_found = true;
			for (int j = 0; j < ul->users[i].auth.count; j++) {
				if (strcmp(ul->users[i].auth.authorized_keys[j].name, key_name) == 0) {
					key_found = true;

					size_t tmp_len = 0;
					tmp_len = strlen(data_alg);
					if(flag) {
						ul->users[i].auth.authorized_keys[j].key_data = strndup(data_alg, tmp_len + 1);
						goto end;
					} else {
						ul->users[i].auth.authorized_keys[j].algorithm = strndup(data_alg, tmp_len + 1);
						goto end;	
					}
				}
			}
		}
	}
end:
	if(!local_user_found || !key_found) {
		return -1;
	}

	return 0;
}

int local_user_add_algorithm(local_user_list_t *ul, char *name, char *key_name, char *algorithm)
{
	int flag = 0;
	int error = 0;
	error = add_algorithm_key_data(ul, name, key_name, algorithm, flag);
	if (error) {
		fprintf(stderr, "Error : Failed to add algorithm - %s\n", strerror(errno));
	}
	return error;
}

int local_user_add_key_data(local_user_list_t *ul, char *name, char *key_name, char *key_data)
{
	int flag = 1;
	int error = 0;
	error = add_algorithm_key_data(ul, name, key_name, key_data, flag);
	if (error) {
		fprintf(stderr, "Error : Failed to add key data - %s\n", strerror(errno));
	}
	return error;
}

void clear_string(char *str)
{
	size_t str_len = 0;
	
	str_len = strlen(str);
	for (size_t i = 0; i < str_len; i++) {
		str[i] = 0;
	}

}

int get_key_info(char *in_dir, local_user_list_t *ul, int i)
{
	FILE *entry_file;
	DIR* FD;
	struct dirent* in_file;
	int error = 0;
	
	char* file_path;
	char *line1 = NULL;
	char *line2 = NULL;
	size_t in_file_len = 0;
	size_t string_len = 0;

	if ((FD = opendir(in_dir)) == NULL) {
		fprintf(stderr, "Error : Failed to open input directory %s - %s\n", in_dir, strerror(errno));
		return 1;
	} else {
		while ((in_file = readdir(FD))) {
			char key_name[100] = {0};
			in_file_len = strlen(in_file->d_name);
			if (in_file_len <= 4) {
				continue;
			}

			if (has_pub_extension(in_file->d_name)) {
				string_len = strlen(in_dir) + strlen("/") + strlen(in_file->d_name) + 1;
				file_path = xmalloc(string_len);
				if (snprintf(file_path, string_len, "%s/%s", in_dir, in_file->d_name) < 0) {
					goto fail;
				} 

				entry_file = fopen(file_path, "r");
				if (entry_file == NULL) {
					fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
       					return 1;
				}

				//adding key: key_name is name of file
				snprintf(key_name, strlen(in_file->d_name)+1, "%s", in_file->d_name);

				error = local_user_add_key(ul, ul->users[i].name, key_name);
				if (error) {
					fprintf(stderr, "Error : Failed to add key - %s\n", strerror(errno));
					fclose(entry_file);
					return 1;
				}
				//reading .pub file
				line1 = xmalloc(MAX_ALG_SIZE * sizeof(char));
				line2 = xmalloc(MAX_KEY_DATA_SIZE * sizeof(char));
				rewind(entry_file);
				fscanf(entry_file, "%s %s", line1, line2);
	
				local_user_add_algorithm(ul, ul->users[i].name, key_name, line1);

				local_user_add_key_data(ul, ul->users[i].name, key_name, line2);

				FREE_SAFE(file_path);
				FREE_SAFE(line1);
				FREE_SAFE(line2);
				fclose(entry_file);
				
			}
		}
	}

	closedir(FD);

	return 0;
fail:
	closedir(FD);
	return -1;
}

int add_existing_local_users(local_user_list_t *ul)
{
	int i = 0;
	int error1 = 0;
	int counting_flag = 0;
	struct passwd *pwd = {0};
	struct spwd *pwdshd = {0};
	char* in_dir;
	size_t string_len = 0;

	setpwent();

	// adding username
	while ((pwd = getpwent()) != NULL) {
		if ((pwd->pw_uid >= 1000 && strncmp(pwd->pw_dir, HOME_PATH, strlen(HOME_PATH)) == 0) || (pwd->pw_uid == 0)){ 
			local_user_add_user(ul, pwd->pw_name);			
		}
	} 
	// adding password
	while((pwdshd = getspent()) != NULL) {
		for(i = 0; i< ul->count; i++) {
			if(strncmp(ul->users[i].name, pwdshd->sp_namp, strlen(pwdshd->sp_namp)) == 0) {
				local_user_set_password(ul, pwdshd->sp_namp, pwdshd->sp_pwdp);
				counting_flag++;
				if(counting_flag == ul->count) {
					goto exit_adding_pass;
				}
				break;
			}
		}
	} 
exit_adding_pass:
	for(i = 0; i< ul->count; i++) {
		if (strncmp(ul->users[i].name, "root", 4) == 0) {
			string_len = strlen ("/root/.ssh") + 1;
			in_dir = xmalloc(string_len);
			if (snprintf(in_dir, string_len, "/root/.ssh") < 0) {
				goto fail;
			} 
			error1 = get_key_info(in_dir, ul, i);
			if(error1) {
				FREE_SAFE(in_dir);
				continue;
			}

		} else {
			string_len = strlen ("/home/") + strlen(ul->users[i].name) + strlen("/.ssh")+ 1;
			in_dir = xmalloc(string_len);
			if (snprintf(in_dir, string_len, "/home/%s/.ssh", ul->users[i].name) < 0) {
				goto fail;
			} 
			
			error1 = get_key_info(in_dir, ul, i);
			if(error1) {
				FREE_SAFE(in_dir);
				continue;
			}
		}
		FREE_SAFE(in_dir);
	}

	endpwent();

	return 0;

fail:
	if (in_dir != NULL) {
		FREE_SAFE(in_dir);
	}

	endpwent();

	return -1;
}

int local_user_list_init(local_user_list_t **ul)
{
	int error = 0;
	*ul = xmalloc(sizeof(local_user_list_t));
	
	for (int i = 0; i < MAX_LOCAL_USERS; i++) {
		local_user_init(&(*ul)->users[i]);
	}
	(*ul)->count = 0;

	error = add_existing_local_users(*ul);
	if (error) {
		fprintf(stderr, "add_existing_local_users error : %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

void local_user_list_free(local_user_list_t *ul)
{
	for (int i = 0; i < ul->count; i++) {
		local_user_free(&ul->users[i]);
	}
}
