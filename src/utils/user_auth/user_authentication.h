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
#include "utarray.h"

#define MAX_LOCAL_USERS 100 // TODO: update if needed
#define MAX_AUTH_KEYS 10	// TODO: update if needed
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

typedef struct {
	char *name;
	char *algorithm;
	char *key_data;
} authorized_key_t;

typedef struct {
	char *name;
	char *password;
	bool nologin;
	UT_array *auth_keys;
} local_user_t;

// authorized_key setters and helpers

void authorized_key_init(authorized_key_t *key);
int authorized_key_set_name(authorized_key_t *key, const char *name);
int authorized_key_set_algorithm(authorized_key_t *key, const char *algorithm);
int authorized_key_set_key_data(authorized_key_t *key, const char *key_data);
void authorized_key_free(authorized_key_t *key);

void authorized_key_array_init(UT_array **keys);
int authorized_key_array_add_key(UT_array **keys, const char *key);
int authorized_key_array_set_algorithm(UT_array **keys, const char *key, const char *algorithm);
int authorized_key_array_set_key_data(UT_array **keys, const char *key, const char *algorithm);
int authorized_key_array_set_ssh_key(UT_array **keys, const char *dir);
void authorized_key_array_free(UT_array **keys);

// local_user setters and helpers

void local_user_init(local_user_t *user);
int local_user_set_name(local_user_t *user, const char *name);
int local_user_set_password(local_user_t *user, const char *password);
int local_user_set_nologin(local_user_t *user, bool nologin);
int local_user_get_key_info(local_user_t *user, const char *dir);
void local_user_free(local_user_t *user);

int local_user_array_init(UT_array **users);
int local_user_array_add_existing(UT_array **users);
int local_user_array_add_user(UT_array **users, const char *name);
int local_user_array_set_password(UT_array **users, const char *name, const char *password);
int local_user_array_add_key(UT_array **users, const char *name, const char *key);
int local_user_array_set_key_algorithm(UT_array **users, const char *name, const char *key, const char *algorithm);
int local_user_array_set_key_data(UT_array **users, const char *name, const char *key, const char *key_data);
int local_user_array_set_new_users(UT_array **users);
int local_user_array_delete_users(UT_array **users);
void local_user_array_free(UT_array **users);

// other helpers
bool has_pub_extension(char *name);

#endif /* USER_AUTHENTICATION_H_ONCE */
