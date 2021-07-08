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

#ifndef USER_AUTHENTICATION_H_ONCE
#define USER_AUTHENTICATION_H_ONCE

#include <inttypes.h>
#include <stdbool.h>

#define MAX_LOCAL_USERS 100 // TODO: update if needed
#define MAX_AUTH_KEYS 10 // TODO: update if needed
#define MAX_USERNAME_LEN LOGIN_NAME_MAX
#define MAX_ALG_SIZE 50
#define MAX_KEY_DATA_SIZE 16384 // maximum RSA key size
#define ROOT_PATH "/root"
#define HOME_PATH "/home"
#define SLASH "/"
#define SSH "/.ssh"

#define ROOT_USERNAME "root"
#define USER_TEMP_PASSWD_FILE "/tmp/tmp_passwd"
#define USER_TEMP_SHADOW_FILE "/tmp/tmp_shadow"
#define PASSWD_FILE "/etc/passwd"
#define SHADOW_FILE "/etc/shadow"
#define PASSWD_BAK_FILE PASSWD_FILE ".bak"
#define SHADOW_BAK_FILE SHADOW_FILE ".bak"

typedef struct local_user_list_s local_user_list_t;

typedef struct {
	char *name;
	char *algorithm;
	char *key_data;
} authorized_key_t;

typedef struct {
	authorized_key_t authorized_keys[MAX_AUTH_KEYS];
	uint8_t count;
} authorized_key_list_t;

typedef struct {
	char *name;
	char *password;
	bool nologin;
	authorized_key_list_t auth;
	//bool delete_user = false;
} local_user_t;

struct local_user_list_s {
	local_user_t users[MAX_LOCAL_USERS];
	uint8_t count;
};

int set_new_users(local_user_list_t *ul);
int get_existing_users_from_passwd(char *existing_users[], int *num_users);

int local_user_list_init(local_user_list_t **ul); 
void local_user_list_free(local_user_list_t *ul);

void local_user_init(local_user_t *u);
void local_user_free(local_user_t *u);
int local_user_add_user(local_user_list_t *ul, char *name);

int add_existing_local_users(local_user_list_t *ul);

int local_user_set_password(local_user_list_t *ul, char *name, char *password);
int local_user_add_key(local_user_list_t *ul, char *name, char *key_name);
int local_user_add_algorithm(local_user_list_t *ul, char *name, char *key_name, char *algorithm);
int local_user_add_key_data(local_user_list_t *ul, char *name, char *key_name, char *key_data);

void authorized_key_list_init(authorized_key_list_t *ul);
void authorized_key_list_free(authorized_key_list_t *ul);
void authorized_key_init(authorized_key_t *k);
void authorized_key_free(authorized_key_t *k);

int delete_users(local_user_list_t *ul);
int remove_user_entry(char *name);
int remove_line_from_file(char *orig, char *tmp, char *backup, char *username);
int remove_ssh_file(char *username, char *filename);
int remove_home_dir(char *username);

int create_dir(char *dir_path, char *username);
int writing_to_key_file(char* in_dir, char *key_name, char *key_algorithm, char *key_data);

bool has_pub_extension(char *name);
// int add_pub_extension(char *name);
void remove_file_name_extension(char *name);
int add_algorithm_key_data(local_user_list_t *ul, char *name, char *key_name, char *data_alg, int flag);
void edit_path(char *new_path, char *old_path, char *name);
int get_key_info(char *in_dir, local_user_list_t *ul, int i);


int set_passwd_file(char *username);
int set_shadow_file(char *username, char *password);
int set_ssh_key(authorized_key_list_t *user_auth, char *ssh_dir_path);
int copy_file(char *src, char *dst);
int create_home_dir(char *username, char **home_dir_path);
int create_ssh_dir(char *username, char *home_dir_path, char **ssh_dir_path);
int set_owner(char *path);

void print_test_fja(local_user_list_t *p);

#endif /* USER_AUTHENTICATION_H_ONCE */
