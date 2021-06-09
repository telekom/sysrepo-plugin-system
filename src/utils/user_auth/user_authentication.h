#ifndef USER_AUTHENTICATION_H_ONCE
#define USER_AUTHENTICATION_H_ONCE

#include <inttypes.h>
#include <stdbool.h>

#define MAX_LOCAL_USERS 100 // TODO: update if needed
#define MAX_AUTH_KEYS 10 // TODO: update if needed

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
	authorized_key_list_t auth;
	//bool delete_user = false;
} local_user_t;

struct local_user_list_s {
	local_user_t users[MAX_LOCAL_USERS];
	uint8_t count;
};

int set_new_users(local_user_list_t *ul);

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


bool has_pub_extension(char *name);
// int add_pub_extension(char *name);
void remove_file_name_extension(char *name);
int add_algorithm_key_data(local_user_list_t *ul, char *name, char *key_name, char *data_alg, int flag);
void clear_string(char *str);
void edit_path(char *new_path, char *old_path, char *name);
int get_key_info(char *in_dir, local_user_list_t *ul, int i);


int set_passwd_file(local_user_list_t *ul, char **temp_array, int *temp_array_len);
int set_key(local_user_list_t *ul);
int copy_file(char *src, char *dst);

void print_test_fja(local_user_list_t *p);

#endif /* USER_AUTHENTICATION_H_ONCE */
