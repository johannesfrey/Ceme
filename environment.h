#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "memory.h"

extern const int ENVIRONMENT_INITIAL_SIZE;

typedef enum {
    ENTRY,
    VALUE
} env_get_t; 

typedef enum {
    CREATE,
    MODIFY
} env_store_t; 

void init_global_env();

object_p global_env_get(env_get_t get_type, object_p key);
void global_env_store(env_store_t store_type, object_p key, object_p value);

object_p local_env_get(env_get_t get_type, object_p lookup_env, object_p key);
void local_env_store(env_store_t store_type, object_p lookup_env, object_p key, object_p value);

object_p env_get(env_get_t get_type, object_p env, object_p key);
void env_store(env_store_t store_type, object_p env, object_p key, object_p value);

#endif
