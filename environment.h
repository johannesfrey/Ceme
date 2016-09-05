#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "memory.h"

void init_global_env();

object_p env_get_entry(object_p env, object_p key);
object_p env_get_value(object_p env, object_p key);
void env_set(object_p env, object_p key, object_p value);
void env_put(object_p env, object_p key, object_p value);

object_p global_env_get_entry(object_p key);
object_p global_env_get_value(object_p key);
void global_env_set(object_p key, object_p value);
void global_env_put(object_p key, object_p value);

object_p local_env_get_entry(object_p lookup_env, object_p key);
object_p local_env_get_value(object_p lookup_env, object_p key);
void local_env_set(object_p lookup_env, object_p key, object_p value);
void local_env_put(object_p lookup_env, object_p key, object_p value);

#endif
