#ifndef SYSTEM_API_CHANGE_H
#define SYSTEM_API_CHANGE_H

#include "context.h"

int system_change_hostname_create(system_ctx_t *ctx, const char *value);
int system_change_hostname_modify(system_ctx_t *ctx, const char *old_value, const char *new_value);
int system_change_hostname_delete(system_ctx_t *ctx);

int system_change_contact_create(system_ctx_t *ctx, const char *value);
int system_change_contact_modify(system_ctx_t *ctx, const char *old_value, const char *new_value);
int system_change_contact_delete(system_ctx_t *ctx);

int system_change_location_create(system_ctx_t *ctx, const char *value);
int system_change_location_modify(system_ctx_t *ctx, const char *old_value, const char *new_value);
int system_change_location_delete(system_ctx_t *ctx);

#endif // SYSTEM_API_CHANGE_H