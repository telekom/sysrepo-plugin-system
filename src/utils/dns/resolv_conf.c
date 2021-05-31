#include "utils/memory.h"
#include <bits/types/res_state.h>
#include <utils/dns/resolv_conf.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <resolv.h>

enum rconf_token_kind_e {
	rconf_token_kind_none = 0,
	rconf_token_kind_comment,
	rconf_token_kind_word,
	rconf_token_kind_kw_nameserver,
	rconf_token_kind_kw_sortlist,
	rconf_token_kind_kw_search,
	rconf_token_kind_kw_options,
};

typedef enum rconf_token_kind_e rconf_token_kind_t;
typedef struct file_content_s file_content_t;
typedef struct rconf_token_s rconf_token_t;
typedef struct rconf_option_s rconf_option_t;

struct file_content_s {
	char *data;
	unsigned long size;
};

struct rconf_token_s {
	char *value;
	rconf_token_kind_t kind;
};

struct rconf_option_s {
	char *name;
	int val; // optional -> most options are only booleans, except attempts, timeout and ndots
};

// static helper functions
static inline file_content_t
load_file(const char *fpath);

// token adding functions
static rconf_error_t add_token_string(rconf_token_t **tok_ls, int *tok_n, char *curr, char *last, bool comment);
static rconf_error_t add_nameserver(rconf_t *cfg, int *idx, rconf_token_t *tokens, int tokens_n);
static rconf_error_t add_search(rconf_t *cfg, int *idx, rconf_token_t *tokens, int tokens_n);
static rconf_error_t set_options(rconf_t *cfg, int *idx, rconf_token_t *tokens, int tokens_n);
static inline rconf_option_t get_option_value(char *opt);

///// API functions and implementations

void rconf_init(rconf_t *cfg)
{
	cfg->nameserver_n = 0;
	cfg->search_n = 0;
	cfg->sortlist_n = 0;

	memset(cfg->nameserver, 0, sizeof(cfg->nameserver));
	memset(cfg->search, 0, sizeof(cfg->search));
	memset(cfg->sortlist, 0, sizeof(cfg->sortlist));

	cfg->options.ndots = 1;
	cfg->options.timeout = RES_TIMEOUT;
	cfg->options.attempts = RES_DFLRETRY;
}

rconf_error_t rconf_load_file(rconf_t *cfg, const char *fpath)
{
	rconf_error_t err = rconf_error_none;
	file_content_t fc = {0};
	rconf_token_t *tokens = NULL;
	int tokens_n = 0;
	char *curr_ptr = NULL, *last_ptr = NULL;

	fc = load_file(fpath);

	if (fc.data == NULL) {
		// error loading file
		err = rconf_error_loading_file;
		goto err_out;
	}

	// got the data -> parse into lines
	enum {
		tokenization_state_normal = 0,
		tokenization_state_comment,
	} tok_state = tokenization_state_normal;

	curr_ptr = last_ptr = fc.data;
	while (*curr_ptr != 0) {
		const char C = *curr_ptr;
		switch (tok_state) {
			case tokenization_state_normal:
				switch (C) {
					case ' ':
						if (curr_ptr - last_ptr > 0) {
							err = add_token_string(&tokens, &tokens_n, curr_ptr, last_ptr, false);
						}
						last_ptr = curr_ptr + 1;
						break;
					case '\n':
						if (curr_ptr - last_ptr > 0) {
							err = add_token_string(&tokens, &tokens_n, curr_ptr, last_ptr, false);
						}
						last_ptr = curr_ptr + 1;
						break;
					case '#':
						tok_state = tokenization_state_comment;
						last_ptr = curr_ptr + 1;
						break;
					default:
						break;
				}
				break;
			case tokenization_state_comment:
				switch (C) {
					case '\n':
						// break from a comment, save it and set new last_ptr
						err = add_token_string(&tokens, &tokens_n, curr_ptr, last_ptr, true);
						tok_state = tokenization_state_normal;
						last_ptr = curr_ptr + 1;
						break;
					default:
						break;
				}
				break;
		}
		++curr_ptr;
	}

	free(fc.data);

	if (err != rconf_error_none) {
		// error occured while creating tokens -> cleanup
		goto err_out;
	}

	// go through tokens and form line structures
	for (int i = 0; i < tokens_n; i++) {
		rconf_token_t *tok_ptr = tokens + i;
		switch (tok_ptr->kind) {
			case rconf_token_kind_none:
				break;
			case rconf_token_kind_comment:
				// check for metadata -> add later
				break;
			case rconf_token_kind_word:
				// error -> unknown token found
				break;
			case rconf_token_kind_kw_nameserver:
				// add nameserver
				err = add_nameserver(cfg, &i, tokens, tokens_n);
				break;
			case rconf_token_kind_kw_sortlist:
				break;
			case rconf_token_kind_kw_search:
				err = add_search(cfg, &i, tokens, tokens_n);
				break;
			case rconf_token_kind_kw_options:
				err = set_options(cfg, &i, tokens, tokens_n);
				break;
		}
		if (err != rconf_error_none) {
			break;
		}
	}

	if (err != rconf_error_none) {
		goto err_out;
	}

err_out:
	for (int i = 0; i < tokens_n; i++) {
		free(tokens[i].value);
	}
	free(tokens);
	return err;
}

rconf_error_t rconf_export(rconf_t *cfg, const char *fpath)
{
	rconf_error_t err = rconf_error_none;
	FILE *fptr = fopen(fpath, "w");

	if (fptr == NULL) {
		err = rconf_error_loading_file;
	} else {
		rconf_print(cfg, fptr);
		fclose(fptr);
	}
	return err;
}

rconf_error_t rconf_set_nameserver(rconf_t *cfg, int idx, char *nameserver, int replace)
{
	rconf_error_t err = rconf_error_none;
	if (idx < MAXNS) {
		if (cfg->nameserver[idx] == NULL) {
			err = rconf_add_nameserver(cfg, nameserver);
		} else if (cfg->nameserver[idx] != NULL && replace) {
			FREE_SAFE(cfg->nameserver[idx]);
			cfg->nameserver[idx] = xstrdup(nameserver);
		} else {
			err = rconf_error_nameserver;
		}
	} else {
		err = rconf_error_nameserver;
	}
	return err;
}

rconf_error_t rconf_add_nameserver(rconf_t *cfg, char *nameserver)
{
	rconf_error_t err = rconf_error_none;
	if (cfg->nameserver_n < MAXNS) {
		cfg->nameserver[cfg->nameserver_n] = xstrdup(nameserver);
		++cfg->nameserver_n;
	} else {
		err = rconf_error_nameserver;
	}
	return err;
}

rconf_error_t rconf_add_search(rconf_t *cfg, char *search)
{
	rconf_error_t err = rconf_error_none;
	if (cfg->search_n < MAXDNSRCH) {
		cfg->search[cfg->search_n] = xstrdup(search);
		++cfg->search_n;
	} else {
		err = rconf_error_search;
	}
	return err;
}

rconf_error_t rconf_remove_search(rconf_t *cfg, char *search)
{
	rconf_error_t err = rconf_error_none;
	int found = 0;
	for (int i = 0; i < cfg->search_n; i++) {
		if (strcmp(cfg->search[i], search) == 0) {
			found = 1;
			// remove string at position i -> free_safe + shift the array left one space
			FREE_SAFE(cfg->search[i]);
			for (int j = i + 1; j < cfg->search_n; j++) {
				cfg->search[j - 1] = cfg->search[j];
			}
			// update number of search items in the array
			--cfg->search_n;
			break;
		}
	}
	if (found == false) {
		// no such value found to remove -> error
		err = rconf_error_no_search_found;
	}
	return err;
}

rconf_error_t rconf_set_ndots(rconf_t *cfg, int ndots)
{
	rconf_error_t err = rconf_error_none;
	// check limit for ndots
	if (ndots > RES_MAXNDOTS || ndots <= 0) {
		err = rconf_error_options_ndots;
	} else {
		cfg->options.ndots = ndots;
	}
	return err;
}

rconf_error_t rconf_set_timeout(rconf_t *cfg, int timeout)
{
	rconf_error_t err = rconf_error_none;
	// check limit for timeout
	if (timeout > RES_MAXRETRANS || timeout <= 0) {
		err = rconf_error_options_timeout;
	} else {
		cfg->options.timeout = timeout;
	}
	return err;
}

rconf_error_t rconf_set_attempts(rconf_t *cfg, int attempts)
{
	rconf_error_t err = rconf_error_none;
	// check limit for attempts
	if (attempts > RES_MAXRETRY || attempts <= 0) {
		err = rconf_error_options_attempts;
	} else {
		cfg->options.attempts = attempts;
	}
	return err;
}

const char *rconf_error2str(rconf_error_t err)
{
	const char *errs[] = {
		[rconf_error_none] = "No error",
		[rconf_error_loading_file] = "Error loading file",
		[rconf_error_alloc] = "Error with allocation",
		[rconf_error_invalid_format] = "Invalid format in the file",
		[rconf_error_nameserver] = "Error parsing nameserver option",
		[rconf_error_search] = "Error parsing search option",
		[rconf_error_sortlist] = "Error parsing sortlist option",
		[rconf_error_options_ndots] = "Invalid ndots option",
		[rconf_error_options_timeout] = "Invalid timeout option",
		[rconf_error_options_attempts] = "Invalid attempts option",
		[rconf_error_no_search_found] = "No such search option found",
	};
	const char *err_unknown = "Unknown error code given";
	char *ret_err = (char *) err_unknown;

	if (err >= rconf_error_none && err < rconf_error_MAX) {
		ret_err = (char *) errs[err];
	}
	return (const char *) ret_err;
}

void rconf_print(rconf_t *cfg, FILE *fptr)
{
	fprintf(fptr, "# nameservers\n");
	for (int i = 0; i < cfg->nameserver_n; i++) {
		fprintf(fptr, "nameserver %s\n", cfg->nameserver[i]);
	}
	fprintf(fptr, "\n");
	fprintf(fptr, "# search list\n");
	fprintf(fptr, "search ");
	for (int i = 0; i < cfg->search_n; i++) {
		fprintf(fptr, "%s ", cfg->search[i]);
	}
	fprintf(fptr, "\n\n");
	fprintf(fptr, "# options\n");
	fprintf(fptr, "options timeout:%d attempts:%d ndots:%d\n", cfg->options.timeout, cfg->options.attempts, cfg->options.ndots);
}

void rconf_free(rconf_t *cfg)
{
	for (int i = 0; i < cfg->nameserver_n; i++) {
		free(cfg->nameserver[i]);
	}
	for (int i = 0; i < cfg->search_n; i++) {
		free(cfg->search[i]);
	}
	for (int i = 0; i < cfg->sortlist_n; i++) {
		free(cfg->sortlist[i]);
	}
	rconf_init(cfg);
}

static inline file_content_t load_file(const char *fpath)
{
	file_content_t fc = {0};

	FILE *file = fopen(fpath, "rb");

	if (file != NULL) {
		fseek(file, 0, SEEK_END);
		fc.size = (unsigned long) ftell(file);
		fseek(file, 0, SEEK_SET);
		fc.data = malloc(sizeof(char) * (fc.size + 1));
		if (fc.data) {
			fread(fc.data, sizeof(char), fc.size, file);
			fc.data[fc.size] = 0;
		}
		fclose(file);
	}

	return fc;
}

static inline rconf_error_t add_token_string(rconf_token_t **tok_ls, int *tok_n, char *curr, char *last, bool comment)
{
	rconf_error_t err = rconf_error_none;
	const unsigned long slen = (unsigned long) (curr - last) + 1;

	// allocate new token
	*tok_n += 1;
	*tok_ls = (rconf_token_t *) realloc(*tok_ls, sizeof(rconf_token_t) * (unsigned long) (*tok_n));

	if (*tok_ls == NULL) {
		err = rconf_error_alloc;
		goto err_out;
	}

	// set token value
	rconf_token_t *tptr = &(*tok_ls)[*tok_n - 1];
	tptr->value = (char *) malloc(sizeof(char) * slen);
	memcpy(tptr->value, last, slen - 1);
	tptr->value[slen - 1] = 0;

	if (comment == false) {
		const char *val = tptr->value;
		if (strcmp(val, "nameserver") == 0) {
			tptr->kind = rconf_token_kind_kw_nameserver;
		} else if (strcmp(val, "search") == 0) {
			tptr->kind = rconf_token_kind_kw_search;
		} else if (strcmp(val, "sortlist") == 0) {
			tptr->kind = rconf_token_kind_kw_sortlist;
		} else if (strcmp(val, "options") == 0) {
			tptr->kind = rconf_token_kind_kw_options;
		} else {
			tptr->kind = rconf_token_kind_word;
		}
	} else {
		tptr->kind = rconf_token_kind_comment;
	}

err_out:
	return err;
}

static rconf_error_t add_nameserver(rconf_t *cfg, int *idx, rconf_token_t *tokens, int tokens_n)
{
	rconf_error_t err = rconf_error_none;

	// format: nameserver ns_IP

	// check for needed tokens
	if (*idx + 1 < tokens_n && tokens[*idx + 1].kind == rconf_token_kind_word) {
		const char *addr = tokens[*idx + 1].value;

		if (cfg->nameserver_n < MAXNS) {
			cfg->nameserver[cfg->nameserver_n] = xstrdup(addr);
			++cfg->nameserver_n;
		} else {
			// too many nameservers in the config -> error
			err = rconf_error_nameserver;
		}
		*idx += 1;
	} else {
		err = rconf_error_invalid_format;
		goto err_out;
	}

err_out:
	return err;
}
static rconf_error_t add_search(rconf_t *cfg, int *idx, rconf_token_t *tokens, int tokens_n)
{
	rconf_error_t err = rconf_error_none;

	// format: search search_list

	if (*idx + 1 < tokens_n && tokens[*idx + 1].kind == rconf_token_kind_word) {
		// iterate from next index until a new keyword is found
		int i = *idx + 1;
		for (; i < tokens_n; i++) {
			const rconf_token_t *tok = tokens + i;
			if (tok->kind != rconf_token_kind_word) {
				break;
			}
			// add to search if possible
			if (cfg->search_n < MAXDNSRCH) {
				cfg->search[cfg->search_n] = xstrdup(tok->value);
				++cfg->search_n;
			} else {
				err = rconf_error_search;
				break;
			}
		}
		*idx = i;
	} else {
		err = rconf_error_invalid_format;
		goto err_out;
	}

err_out:
	return err;
}
static rconf_error_t set_options(rconf_t *cfg, int *idx, rconf_token_t *tokens, int tokens_n)
{
	rconf_error_t err = rconf_error_none;

	// format options option ...
	if (*idx + 1 < tokens_n && tokens[*idx + 1].kind == rconf_token_kind_word) {
		int i = *idx + 1;

		for (; i < tokens_n; i++) {
			const rconf_token_t *tok = tokens + i;
			if (tok->kind != rconf_token_kind_word) {
				break;
			}

			// get current option value and possibly a number and set accordingly
			const rconf_option_t opt = get_option_value(tok->value);
			if (strncmp(opt.name, "attempts", sizeof("attempts")) == 0) {
				cfg->options.attempts = opt.val;
			} else if (strncmp(opt.name, "timeout", sizeof("timeout")) == 0) {
				cfg->options.timeout = opt.val;
			} else if (strncmp(opt.name, "ndots", sizeof("ndots")) == 0) {
				cfg->options.ndots = opt.val;
			}
			free(opt.name);
		}
	} else {
		err = rconf_error_invalid_format;
		goto err_out;
	}
err_out:
	return err;
}

static rconf_option_t get_option_value(char *opt)
{
	// from a given option just return the structure containing the option name + optional value
	rconf_option_t o = {0};
	// split opt string by ':'
	char *iter = opt;
	for (; *iter != 0; iter++) {
		if (*iter == ':') {
			break;
		}
	}
	if (*iter == ':') {
		// two parts => name:value
		const unsigned long len = (unsigned long) (iter - opt);
		o.name = (char *) malloc(sizeof(char) * (len + 1));
		memcpy(o.name, opt, len);
		o.name[len] = 0;
		o.val = atoi(iter + 1);
	} else {
		// single part
		o.name = xstrdup(opt);
	}
	return o;
}
