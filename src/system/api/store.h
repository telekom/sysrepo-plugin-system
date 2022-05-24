#ifndef SYSTEM_API_STORE_H
#define SYSTEM_API_STORE_H

#include "context.h"

int system_store_hostname(system_ctx_t *ctx, const char *hostname);
int system_store_contact(system_ctx_t *ctx, const char *contact);
int system_store_location(system_ctx_t *ctx, const char *location);

#endif // SYSTEM_API_STORE_H