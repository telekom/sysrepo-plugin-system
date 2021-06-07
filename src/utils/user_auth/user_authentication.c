#include "user_authentication.h"
#include "../memory.h"
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


#define MAX_USERNAME_LEN 50
#define MAX_ALG_SIZE 50
#define MAX_KEY_DATA_SIZE 1000
#define ROOT_PATH "/root"
#define HOME_PATH "/home"
#define SLASH "/"
#define SSH "/.ssh" 

#define ROOT_USERNAME "root"
#define USER_TEMP_FILE "/tmp/tempfile"
#define USER_TEMP_SHADOW_FILE "/tmp/tempshadowfile"
#define PASSWD_FILE "/etc/passwd"
#define SHADOW_FILE "/etc/shadow"
#define PASSWD_BAK_FILE PASSWD_FILE ".bak"
#define SHADOW_BAK_FILE SHADOW_FILE ".bak"

uid_t uid = 0;
gid_t gid = 0;
//////////////////////////////////////////////////////////////////////////////////////
//pomocne fje
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
	// TODO: please refactor
	size_t username_len = 0;
	int flag = 0;
	int temp_array_len = 0;
	char **temp_array = NULL; 
	struct spwd *shd = {0};
	struct spwd s = {0};
	FILE *tmp_shf = NULL;
	
	temp_array = xmalloc(MAX_LOCAL_USERS * sizeof(char*));
	for (int i = 0; i < MAX_LOCAL_USERS; i++) {
		temp_array[i] = (char*)xmalloc(MAX_USERNAME_LEN * sizeof(char));
	}

	if (set_passwd_file(ul, temp_array, &temp_array_len)) {
		fprintf(stderr, "Error : Failed to set passwd file - %s\n", strerror(errno));
		goto fail;
	}

//////////////////////////////////////////////////////////////////////////////////////
//ZA SHADOW FILE
//temp_array je vec podesen, njega ne treba nanovo mjenjati, jer sve sto se nadodavalo za passwd, treba ici i u shadow file

	tmp_shf = fopen(USER_TEMP_SHADOW_FILE, "w");
	if (!tmp_shf) {
		goto fail;
	}

	endspent(); 

	shd = getspent();
	if (shd == NULL) {
		goto fail;
	}
//////////////////////////////////////////////////////////////////////////////////////

	do {
		if (putspent(shd, tmp_shf) != 0) {
			goto fail;
		}

	} while ((shd = getspent()) != NULL);

	//dosao je do kraja, sada treba dodati nove usere

	for (int i = 0; i < ul->count; i++) {
		flag = 0;
		for (int j = 0; j < temp_array_len; j++) {
			if (strncmp(temp_array [j], ul->users[i].name, strlen(ul->users[i].name)) == 0) {
				flag = 1;
				goto next_3;	
			}
		}
next_3:
		if (!flag) {	//u ovome if-u se nadodaju novi useri
						//POGLEDATI TREBA LI JOS DODATI I PERMISSIONE
			username_len = strlen (ul->users[i].name) + 1;
			s.sp_namp = strndup(ul->users[i].name, username_len);

			username_len = strlen (ul->users[i].password) + 1;
			s.sp_pwdp = strndup(ul->users[i].password, username_len);

			s.sp_lstchg = 18655;
			s.sp_max = 99999;
			s.sp_min = 0;
			s.sp_warn = 7;

		/*	s.sp_expire = 0;
			s.sp_flag = 0;
			s.sp_inact = 0;
		*/
			if (putspent(&s, tmp_shf) != 0) {
				goto fail;
			}
			FREE_SAFE(s.sp_namp);
			FREE_SAFE(s.sp_pwdp); 
		}
	}

///////////////////////////////////////////////////////////////////////////////////////////////
	
	fclose(tmp_shf);
	tmp_shf = NULL;

	// create a backup file of /etc/passwd
	if (copy_file(SHADOW_FILE, SHADOW_BAK_FILE) != 0) {
		printf("copy_file error: %s", strerror(errno));
		goto fail;
	}

	// copy the temp file to /etc/passwd
	if (copy_file(USER_TEMP_SHADOW_FILE, SHADOW_FILE) != 0) {
		printf("copy_file error: %s", strerror(errno));
		goto fail;
	}

	// remove the temp file
	if (remove(USER_TEMP_SHADOW_FILE) != 0)
		goto fail;

//////////////////////////////////////////////////////////////////////////////////////
//DODAVANJE KEYEVA
	if (set_key(ul)) {
		printf("set_key error: %s", strerror(errno));
		goto fail;
	}

	for (int i = 0; i < MAX_LOCAL_USERS; i++){
		if (temp_array[i] != NULL)
			FREE_SAFE(temp_array[i]);
	}
	FREE_SAFE(temp_array);

	return 0;

fail:
	for (int i = 0; i < MAX_LOCAL_USERS; i++){
		if (temp_array[i] != NULL)
			FREE_SAFE(temp_array[i]);
	}
	FREE_SAFE(temp_array);

	if (access(PASSWD_FILE, F_OK) != 0 )
		rename(PASSWD_BAK_FILE, PASSWD_FILE);
	
	if (tmp_shf != NULL)
		fclose(tmp_shf);

	return -1;
}

int writing_to_key_file(local_user_list_t *ul, int i, int j, char* in_dir) 
{
	size_t string_len = 0;
	FILE *file_key = NULL;
	char* file_path = NULL;

	string_len = strlen(in_dir) + strlen("/") + strlen(ul->users[i].auth.authorized_keys[j].name) + 1;
	file_path = xmalloc(string_len);
	snprintf(file_path, string_len, "%s/%s", in_dir, ul->users[i].auth.authorized_keys[j].name);
	file_key = fopen(file_path, "w");

	FREE_SAFE(file_path);

	fprintf(file_key, "%s\n", ul->users[i].auth.authorized_keys[j].algorithm);
	fprintf(file_key, "%s", ul->users[i].auth.authorized_keys[j].key_data);
	if (file_key == NULL) {
		fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
		goto fail;
	} else {
		fclose(file_key);
		file_key = NULL;
		return 0;
	}
fail:
	return -1;
	
}

int set_key(local_user_list_t *ul)
{
	int i = 0;
	int j = 0;
	char *in_dir = NULL;
	size_t string_len = 0;
	DIR *FD;
	int flag = 0;
	struct dirent *in_file;
	size_t in_file_len = 0;
	char *file_key_name = NULL;
	struct stat st = {0};

	for (i = 0; i < ul->count; i++) {
		if (strcmp(ul->users[i].name, ROOT_USERNAME) == 0) {
			string_len = strlen("/") + strlen(ul->users[i].name) + strlen("/.ssh") + 1;
			in_dir = xmalloc(string_len);
			if (snprintf(in_dir, string_len, "%s/.ssh", ROOT_PATH) < 0) {
				goto fail;
			}
		} else {

			string_len = strlen(ul->users[i].name) + strlen("/home/") + strlen("/.ssh") + 1;
			in_dir = xmalloc(string_len);
			if (snprintf(in_dir, string_len, "/home/%s/.ssh", ul->users[i].name) < 0) {
				goto fail;
			}
		}
		
		if (stat(in_dir, &st) == -1) {
			printf("Creating new folder: \n");
    		mkdir(in_dir, 0700);
		}	

		if ((FD = opendir(in_dir)) == NULL) {
    		fprintf(stderr, "Error : Failed to open input directory %s - %s\n", in_dir, strerror(errno));
			FREE_SAFE(in_dir);
			goto fail;        	
		} else {
			for(j = 0; j < ul->users[i].auth.count; j++) {
				flag = 0;
				while ((in_file = readdir(FD))) {
					in_file_len = strlen(in_file->d_name);
					if (in_file_len <= 4) {
						continue;
					}

					string_len = strlen(in_file->d_name) + 1;
					file_key_name = xmalloc(string_len);
					if (snprintf(file_key_name, string_len, "%s", in_file->d_name) < 0) {
						goto fail;
					}

					if (strncmp(file_key_name, ul->users[i].auth.authorized_keys[j].name, strlen(ul->users[i].auth.authorized_keys[j].name)) == 0) {
						flag = 1;
						if(!writing_to_key_file(ul, i, j, in_dir)) {
							FREE_SAFE(file_key_name);
							continue;
						} else {
							goto fail;
						}
					}
					FREE_SAFE(file_key_name);
				}
				if (!flag) {
					if(!writing_to_key_file(ul, i, j, in_dir)) {
						continue;
					} else {
						goto fail;
					}
				}
			}
		}
		FREE_SAFE(in_dir);
		closedir(FD);
	}
	
	return 0;
fail:
	FREE_SAFE(in_dir);

	if (file_key_name != NULL) {
		FREE_SAFE(file_key_name);
	}

	if (FD != NULL) {
		closedir(FD);
	}

	return -1;
}

int set_passwd_file(local_user_list_t *ul, char **temp_array, int *temp_array_len)
{
	int flag = 0;
	int read_fd = -1;
	int write_fd = -1;
	struct stat st = {0};
	struct stat stat_buf = {0};
	struct passwd *pwd = {0};
	struct passwd p = {0};
	off_t offset = 0;
	size_t username_len = 0;
	FILE *tmp_pwf = NULL; // temporary passwd file

	tmp_pwf = fopen(USER_TEMP_FILE, "w");
	if (!tmp_pwf) {
		printf("error while opening temporary passwd file: %s", strerror(errno));
		goto fail;
	}
	endpwent(); 

	pwd = getpwent();
	if (pwd == NULL) {
		goto fail;
	}
///////////////////////////////////////////////////////////////////////////////////////
	do {
		for(int i = 0; i < ul->count; i++) {
			if (strncmp(pwd->pw_name, ul->users[i].name, strlen(ul->users[i].name)) == 0) {
				(*temp_array_len) = *temp_array_len + 1;
				if (*temp_array_len > MAX_LOCAL_USERS) {
					goto fail;
				} else {
					temp_array[*temp_array_len - 1] = strndup(ul->users[i].name, strlen(ul->users[i].name));
					goto next_1;
				}
			}
		}
next_1:
		if (putpwent(pwd, tmp_pwf) != 0) {
			goto fail;
		}
	} while ((pwd = getpwent()) != NULL);

	//preparing to add new users
	for (int i = 0; i < ul->count; i++) {
		flag = 0;
		for (int j = 0; j < *temp_array_len; j++) {
			if (strncmp(temp_array [j], ul->users[i].name, strlen(ul->users[i].name)) == 0) {
				flag = 1;
				goto next_2;	
			}
		}
next_2:
		if (!flag) {	//adding_new_users
			username_len = strlen (ul->users[i].name) + 1;
			
			p.pw_name = strndup (ul->users[i].name, username_len);
			p.pw_passwd = strdup("x");
			p.pw_shell = strdup ("bin/bash");
			uid++;
			gid++;
			p.pw_uid = uid;
			p.pw_gid = gid;

			username_len = strlen ("/home/") + strlen(p.pw_name) + 1;
			p.pw_dir = xmalloc(username_len);
			if (snprintf(p.pw_dir, username_len, "/home/%s", p.pw_name) < 0) {
				goto fail;
			} 
			if (stat(p.pw_dir, &st) == -1) {
    			mkdir(p.pw_dir, 0700);
			}
			if (chown(p.pw_dir, p.pw_uid, p.pw_gid)) {
				goto fail;
			}
	
			if (putpwent(&p, tmp_pwf) != 0) {
				goto fail;
			}

			FREE_SAFE(p.pw_name);
			FREE_SAFE(p.pw_passwd);
			FREE_SAFE(p.pw_shell);
			FREE_SAFE(p.pw_dir);
		}
	}

	fclose(tmp_pwf);
	tmp_pwf = NULL;

	// create a backup file of /etc/passwd
	if (rename(PASSWD_FILE, PASSWD_BAK_FILE) != 0)
		goto fail;

	// copy the temp file to /etc/passwd
	read_fd = open(USER_TEMP_FILE, O_RDONLY);
	if (read_fd == -1)
		goto fail;

	if (fstat(read_fd, &stat_buf) != 0)
		goto fail;

	write_fd = open(PASSWD_FILE, O_WRONLY | O_CREAT, stat_buf.st_mode);
	if (write_fd == -1)
		goto fail;

	if (sendfile(write_fd, read_fd, &offset, (size_t)stat_buf.st_size) == -1)
		goto fail;

	// remove the temp file
	if (remove(USER_TEMP_FILE) != 0)
		goto fail;

	close(read_fd);
	close(write_fd);

	return 0;

fail:
	if (tmp_pwf != NULL)
		fclose(tmp_pwf);

	if (read_fd != -1)
		close(read_fd);

	if (write_fd != -1)
		close(write_fd);


	return -1;

}
















int copy_file(char *src, char *dst)
{
	int read_fd = -1;
	int write_fd = -1;
	struct stat stat_buf = {0};
	off_t offset = 0;

	read_fd = open(src, O_RDONLY);
	if (read_fd == -1)
		goto error_out;

	if (fstat(read_fd, &stat_buf) != 0)
		goto error_out;

	write_fd = open(dst, O_WRONLY | O_CREAT, stat_buf.st_mode);
	if (write_fd == -1)
		goto error_out;

	if (sendfile(write_fd, read_fd, &offset, (size_t)stat_buf.st_size) == -1)
		goto error_out;

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
	//ul = xmalloc(sizeof(authorized_key_list_t));
	
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
	//u = xmalloc(sizeof(local_user_t));
	
	u->name = NULL;
	u->password = NULL;

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
	int error = 0;
	size_t tmp_len = strlen(name);

	if (ul->count >= MAX_LOCAL_USERS) {
		error = EINVAL;
	} else {
		ul->users[ul->count].name = strndup(name, tmp_len + 1);
		(ul->count)++;
	}

	return error;
}

int local_user_set_password(local_user_list_t *ul, char *name, char *password)
{
	bool local_user_found = false;

	for (int i = 0; i < ul->count; i++ ) {
		if (strcmp(ul->users[i].name, name) == 0) {
			//if password was already set, free it first, and then reset it
			if (ul->users[i].password != NULL) {
				FREE_SAFE(ul->users[i].password);
			}

			size_t tmp_len = 0;
			tmp_len = strlen(password);
			ul->users[i].password = strndup(password, tmp_len + 1);

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
	//str[0] = '\0';
}

int get_key_info(char *in_dir, local_user_list_t *ul, int i)
{
	FILE *entry_file;
	DIR* FD;
	struct dirent* in_file;
	int error1 = 0;
	
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
			//otvaranje svih .pub datoteka
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
				//clear_string(key_name);
				//strncat(key_name, in_file->d_name, strlen(in_file->d_name));
				snprintf(key_name, strlen(in_file->d_name)+1, "%s", in_file->d_name);
				//ako treba nadodati:
				//remove_file_name_extension(key_name);
				error1 = local_user_add_key(ul, ul->users[i].name, key_name);
				if (error1) {
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

	//adding username
	while ((pwd = getpwent()) != NULL) {
		if ((pwd->pw_uid >= 1000 && strncmp(pwd->pw_dir, HOME_PATH, strlen(HOME_PATH)) == 0) || (pwd->pw_uid == 0)){ 
			if (pwd->pw_uid > uid) {
				uid = pwd->pw_uid;
				gid = uid;
			}
			local_user_add_user(ul, pwd->pw_name);			
		}
	} 
	//adding password
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
	//adding key; u slucaju provjere, zakomentirati ovaj if sa rootom jer je samo 1 username na kompu
	for(i = 0; i< ul->count; i++) {
		
		//clear_string(in_dir);
		if (strncmp(ul->users[i].name, "root", 4) == 0) {
			string_len = strlen ("/root/.ssh") + 1;
			in_dir = xmalloc(string_len);
			if (snprintf(in_dir, string_len, "/root/.ssh") < 0) {
				goto fail;
			} 
			error1 = get_key_info(in_dir, ul, i);
			if(error1) {
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
				continue;
			}
		}
		FREE_SAFE(in_dir);
	}

	return 0;

fail:
	FREE_SAFE(in_dir);
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////
//user list: dovrsiti zadnje linije kod inita i free-a
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
