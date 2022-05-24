#ifndef SYSTEM_API_LOAD_H
#define SYSTEM_API_LOAD_H

#include "common.h"
#include "context.h"

int system_load_hostname(system_ctx_t *ctx, char buffer[SYSTEM_HOSTNAME_LENGTH_MAX]);
int system_load_contact(system_ctx_t *ctx, char buffer[256]);
int system_load_location(system_ctx_t *ctx, char buffer[256]);

#endif // SYSTEM_API_LOAD_H