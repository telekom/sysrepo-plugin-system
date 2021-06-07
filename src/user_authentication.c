/*#include "user_authentication.h"*/
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

#define MAX_LOCAL_USERS 20 // TODO: update if needed
#define MAX_AUTH_KEYS 20 // TODO: update if needed
#define MAX_USERNAME_LEN 50
/*#define NTP_CONFIG_FILE "/etc/user_auth.conf"
#define NTP_TEMP_FILE "/tmp/tmp_user_auth.conf"
#define NTP_BAK_FILE "/etc/user_auth.conf.bak"
*/

void local_user_set_name(struct local_user_s *u, char *name);

struct authorized_key_s {
	char *name;
	char *algorithm;
	char *key_data; // binary
};

struct authorized_key_list_s {
	struct authorized_key_s authorized_keys[MAX_AUTH_KEYS];
	uint8_t count;
};

struct local_user_s {
	char *name;
	char *password;
	struct authorized_key_list_s authorized_keys;
};

struct local_user_list_s {
	struct local_user_s users[MAX_LOCAL_USERS];
	uint8_t count;
};
//////////////////////////////////////////////////////////////////////////////////////
//key:napravljeno: init, free
void authorized_key_init(struct authorized_key_s *s)
{	
	s->name = malloc(sizeof(char));
	s->name[0] = 0;
	s->algorithm = malloc(sizeof(char));
	s->algorithm = 0;
	s->key_data = malloc(sizeof(char));
	s->key_data = 0;
}

void authorized_key_free(struct authorized_key_s *s)
{
	if (s->name) {
		FREE_SAFE(s->name);
	}
	
	if (s->algorithm) {
		FREE_SAFE(s->algorithm);
	}
	
	if (s->key_data) {
		FREE_SAFE(s->key_data);
	}
}
//////////////////////////////////////////////////////////////////////////////////////
//pomocne fje
int add_existing_local_users(struct local_user_list_s *ul)
{
	int i = 0;
	struct passwd *pwd = {0};

	pwd = getpwent();

	if (pwd == NULL) {
		return -1;
	}
	printf("adding existing\n");
	do {
		if (pwd->pw_uid >= 1000 && pwd->pw_dir != "/") { 
			//strncpy(ul->users[i].name, pwd->pw_name, strnlen(pwd->pw_name, MAX_USERNAME_LEN));
			ul->users[i].name = strndup(pwd->pw_name, strnlen(pwd->pw_name, MAX_USERNAME_LEN));
			//local_user_set_name(ul->users[i], pwd->pw_name);
			i++;
		}
		printf("%d\n", i);
	} while ((pwd = getpwent()) != NULL);

	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////
//key list:napravljeno:init, free
void authorized_key_list_init(struct authorized_key_list_s *sl)
{
	sl = malloc(sizeof(struct authorized_key_list_s));
	
	for (int i = 0; i < MAX_AUTH_KEYS; i++) {
		authorized_key_init(&sl->authorized_keys[i]);
	}
	sl->count = 0;
}

void authorized_key_list_free(struct authorized_key_list_s *sl)
{
	for (int i = 0; i < sl->count; i++) {
		authorized_key_free(&sl->authorized_keys[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//user:napravljeno: init, free, add_user, set_name, set_password, add_key
	
void local_user_init(struct local_user_s *sl)
{
	
	sl = malloc(sizeof(struct local_user_s));
	
	sl->name = malloc(sizeof(char));
	sl->name = 0;
	sl->password = malloc(sizeof(char));
	sl->password = 0;
	authorized_key_list_init(&sl->authorized_keys);
	
}

void local_user_free(struct local_user_s *s)
{
	if (s->name) {
		FREE_SAFE(s->name);
	}
	
	if (s->password) {
		FREE_SAFE(s->password);
	}

	authorized_key_list_free(&s->authorized_keys);	
}

void local_user_set_name(struct local_user_s *u, char *name)
{
	size_t tmp_len = strlen(name);
	u->name = strndup(name, tmp_len + 1);
}

int local_user_add_user(struct local_user_list_s *u, char *name)
{
	int error = 0;

	if (u->count >= MAX_LOCAL_USERS) {
		error = EINVAL;
	} else {
		local_user_set_name(&u->users[u->count], name);
		(u->count)++;
	}

	return error;
}

int local_user_set_password(struct local_user_list_s *u, char *name, char *password)
{
	bool local_user_found = false;

	for (int i = 0; i < u->count; i++ ) {
		if (strcmp(u->users[i].name, name) == 0) {
			//if password was already set, free it first, and then reset it
			if (u->users[i].password != NULL) {
				FREE_SAFE(u->users[i].password);
			}

			size_t tmp_len = 0;
			tmp_len = strlen(password);
			u->users[i].password = strndup(password, tmp_len + 1);

			local_user_found = true;
			break;
		}	
	}
	if(!local_user_found) {
		return -1;
	}
	return 0;
}

int local_user_add_key(struct local_user_list_s *u, char *name, char *key_name)
{
	bool local_user_found = false;

	for (int i = 0; i < u->count; i++) {
		if (strcmp(u->users[i].name, name) == 0) {
			local_user_found = true;

			u->users[i].authorized_keys.count++;
			size_t tmp_len = 0;
			tmp_len = strlen(key_name);
			u->users[i].authorized_keys.authorized_keys->name = strndup(key_name, tmp_len + 1);

			break;	
		}		
	}
	if(!local_user_found) {
		return -1;
	}
	return 0;
}

int local_user_add_algorithm(struct local_user_list_s *u, char *name, char *key_name, char *algorithm)
{
	bool local_user_found = false;
	bool key_found = false;

	for (int i = 0; i < u->count; i++) {
		if (strcmp(u->users[i].name, name) == 0) {
			local_user_found = true;
			for (int j = 0; j < u->users->authorized_keys.count; j++) {
				if (strcmp(u->users->authorized_keys.authorized_keys->name, key_name) == 0) {
					key_found = true;

					size_t tmp_len = 0;
					tmp_len = strlen(algorithm);
					u->users[i].authorized_keys.authorized_keys->algorithm = strndup(algorithm, tmp_len + 1);
					goto end;
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

int local_user_add_key_data(struct local_user_list_s *u, char *name, char *key_name, char *key_data)
{
	bool local_user_found = false;
	bool key_found = false;

	for (int i = 0; i < u->count; i++) {
		if (strcmp(u->users[i].name, name) == 0) {
			local_user_found = true;
			for (int j = 0; j < u->users->authorized_keys.count; j++) {
				if (strcmp(u->users->authorized_keys.authorized_keys->name, key_name) == 0) {
					key_found = true;

					size_t tmp_len = 0;
					tmp_len = strlen(key_data);
					u->users[i].authorized_keys.authorized_keys->key_data = strndup(key_data, tmp_len + 1);
					goto end;
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

//////////////////////////////////////////////////////////////////////////////////////
//user list: dovrsiti zadnje linije kod inita i free-a
void local_user_list_init(struct local_user_list_s **sl)
{
	int error = 0;
	*sl = malloc(sizeof(struct local_user_list_s));
	
	for (int i = 0; i < MAX_LOCAL_USERS; i++) {
		local_user_init(&(*sl)->users[i]);
	}
	(*sl)->count = 0;

	error = add_existing_local_users(*sl);
	if (error) {
		printf("ERROR WHILE ADDING EXISTING USERS : %d\n", error);
	}	
}

void local_user_list_free(struct local_user_list_s *sl)
{
	for (int i = 0; i < sl->count; i++) {
		local_user_free(&sl->users[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//testiranje
void print_test_fja(struct local_user_list_s *p, int n, int m)
{
	//for (int i = 0; i < p->count; i++) {
	for (int i = 0; i < n; i++) {
		printf("Username: %s\n", p->users[i].name);
		printf("User pw: %s\n", p->users[i].password);

	//	for (int j = 0; j < p->users[i].authorized_keys.count; i++) {
		for (int j = 0; j < m; j++) {
			printf("auth_key name: %s\n", p->users[i].authorized_keys.authorized_keys[j].name);
			printf("auth_key algorithm: %s\n", p->users[i].authorized_keys.authorized_keys[j].algorithm);
			printf("auth_key key_data: %s\n", p->users[i].authorized_keys.authorized_keys[j].key_data);
			printf("\n");
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////
static struct local_user_list_s *local_users = {0};

int main(void){
	int error1 = 0;
	char name[50];
	char password[100];
	char key_name[100];
	char algorithm[100];
	char key_data[100];
	int i;
	int j;
	bool flag = false;

	printf("main\n");

	local_user_list_init(&local_users);
	print_test_fja(local_users, MAX_LOCAL_USERS, MAX_AUTH_KEYS);
	printf("\n\n\n\n\n");

	//adding user
	printf("Enter username: \n");
	scanf("%s", name);
	error1 = local_user_add_user(local_users, name);
	printf("Error type: %d\n", error1);
	printf("\nNumber of users: %d\n", local_users->count);
	print_test_fja(local_users, local_users->count, local_users->users->authorized_keys.count); //ne isprinta name, aglorithm i key_data jer nema jos fje za dodavanje kljuca
	error1 = 0;

	//setting password
	printf("Enter username for setting password: \n");
	scanf("%s", name);
	printf("Enter new password: \n");
	scanf("%s", password);
	error1 = local_user_set_password(local_users, name, password);
	printf("Error type: %d\n", error1);
	printf("\nNumber of users: %d\n", local_users->count);
	print_test_fja(local_users, local_users->count, local_users->users->authorized_keys.count);
	error1 = 0;

	//setting name of key
	printf("Enter username for setting key_name: \n");
	scanf("%s", name);
	printf("Enter new key_name: \n");
	scanf("%s", key_name);
	error1 = local_user_add_key(local_users, name, key_name);
	printf("Error type: %d\n", error1);
	printf("\nNumber of users: %d\n", local_users->count);
	for(i = 0; i < local_users->count; i++) {
		if (strcmp(local_users->users[i].name, name) == 0) {
			flag = true;
			break;
		}
	}
	if(!flag){
		goto exit;
	}
	printf("\n");
	puts(name);
	printf("Number of keys: %d", local_users->users[i].authorized_keys.count);
	printf("\n");
	print_test_fja(local_users, local_users->count, local_users->users->authorized_keys.count);
	error1 = 0;

	//setting algorithm of key
	flag = false;
	printf("Enter username for setting algorithm: \n");
	scanf("%s", name);

	for(i = 0; i < local_users->count; i++) {
		if (strcmp(local_users->users[i].name, name) == 0) {
			flag = true;
			break;
		}
	}
	if(!flag){
		goto exit;
	}
	flag = false;
	printf("Enter key_name for setting algorithm: \n");
	scanf("%s", key_name);

	for(j = 0; j < local_users->users->authorized_keys.count; j++) {
		if (strcmp(local_users->users[i].authorized_keys.authorized_keys->name, key_name) == 0) {
			flag = true;
			break;
		}
	}
	if(!flag){
		goto exit;
	}
	flag = false;

	printf("Enter algorithm for key: \n");
	scanf("%s", algorithm);
	error1 = local_user_add_algorithm(local_users, name, key_name, algorithm);
	printf("Error type: %d\n", error1);
	printf("\nNumber of users: %d\n", local_users->count);
	printf("\n");
	puts(name);
	printf("Number of keys: %d", local_users->users[i].authorized_keys.count);
	printf("\n");
	print_test_fja(local_users, local_users->count, local_users->users->authorized_keys.count);
	error1 = 0;
	flag = false;

	//setting key_data of key
	flag = false;
	printf("Enter username for setting key_data: \n");
	scanf("%s", name);

	for(i = 0; i < local_users->count; i++) {
		if (strcmp(local_users->users[i].name, name) == 0) {
			flag = true;
			break;
		}
	}
	if(!flag){
		goto exit;
	}
	flag = false;
	printf("Enter key_name for setting key_data: \n");
	scanf("%s", key_name);

	for(j = 0; j < local_users->users->authorized_keys.count; j++) {
		if (strcmp(local_users->users[i].authorized_keys.authorized_keys->name, key_name) == 0) {
			flag = true;
			break;
		}
	}
	if(!flag){
		goto exit;
	}
	flag = false;

	printf("Enter key data for key: \n");
	scanf("%s", key_data);
	error1 = local_user_add_key_data(local_users, name, key_name, key_data);
	printf("Error type: %d\n", error1);
	printf("\nNumber of users: %d\n", local_users->count);
	printf("\n");
	puts(name);
	printf("Number of keys: %d", local_users->users[i].authorized_keys.count);
	printf("\n");
	print_test_fja(local_users, local_users->count, local_users->users->authorized_keys.count);
	error1 = 0;
	flag = false;
exit:
	local_user_list_free(local_users); 
										//  put prefix x in malloc...!
	return 0;

}

