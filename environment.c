#include <stdlib.h>

#include "dbg.h"
#include "memory.h"
#include "environment.h"

#define internal static

const int ENVIRONMENT_INITIAL_SIZE = 511;

object_p global_env;
static int global_env_size = ENVIRONMENT_INITIAL_SIZE;
static int fill_level = 0;

void
init_global_env()
{
    debug("Initializing global environment");
    global_env_size = ENVIRONMENT_INITIAL_SIZE;
    global_env = alloc_global_env(global_env_size);
    memset((void*)(&(global_env->env.bindings)), 0, (sizeof(env_binding_t)*global_env_size));
} 

internal void
global_env_rehash()
{
    debug("Rehash global environment");
    int old_size = global_env_size;
    int new_size = (old_size + 1) * 2 - 1;
    int idx_old_env;

    object_p old_env = global_env;
    object_p new_env;

    new_env = alloc_global_env(new_size);

    for (idx_old_env = 0; idx_old_env < old_size; idx_old_env++) {
        env_binding_t *old_entry = &(old_env->env.bindings[idx_old_env]);

        // hash object for new table and try to insert into next free slot
        if (old_entry != NULL) {
            int new_hash = (int)(old_entry->car);
            int start_idx = new_hash % new_size;
            int next_idx = start_idx;
            object_p peek;

            for (;;) {
                peek = new_env->env.bindings[next_idx].car;
                
                if (peek == NULL) {
                    new_env->env.bindings[next_idx].car = old_entry->car;
                    new_env->env.bindings[next_idx].cdr = old_entry->cdr;
                    break;
                }

                // slot is occupied; try next
                next_idx = (next_idx + 1) % new_size;

                // looped through table but no free slots found!
                check(next_idx != start_idx, "Fatal, no free slots found in global environment!");
            }
        }
    }

    free(old_env);
    global_env = new_env;
    global_env_size = new_size;
    return;

error:
    abort();
}

// Retrieve the value of the global env entry
object_p
global_env_get(env_get_t get_type, object_p key)
{
    debug("Get from global environment");
    int h = (int)key;
    int start_idx = h % global_env_size;
    int next_idx;
    object_p peek;

    next_idx = start_idx;

    for (;;) {
        peek = global_env->env.bindings[next_idx].car;

        if (peek == key) {
            if (get_type == ENTRY) {
                // for whole entry (cons cell) in global env
                return (object_p)&(global_env->env.bindings[next_idx]);
            }
            // only for the value in global env
            return global_env->env.bindings[next_idx].cdr;
        }
        if (peek == NULL) {
            if (get_type == ENTRY) {
                return nil_object;
            }
            return NULL;
        }

        next_idx = (next_idx + 1) % global_env_size;
        check(next_idx != start_idx, "Fatal, no free slots found in global environment!");
    }

error:
    abort();
}

void
global_env_store(env_store_t store_type, object_p key, object_p value)
{
    debug("Store into global environment");
    int h = (int)key;
    int start_idx = h % global_env_size;
    int next_idx;
    object_p peek;

    next_idx = start_idx;

    for (;;) {
        peek = global_env->env.bindings[next_idx].car;

        if (peek == key) {
            // key is present, replace value
            global_env->env.bindings[next_idx].cdr = value;
            return;
        }
        if (peek == NULL) {
            if (store_type == MODIFY) {
                log_err("Key not found!");
                return;
            }
            // CREATE flag: key not present, store key and value
            global_env->env.bindings[next_idx].car = key;
            global_env->env.bindings[next_idx].cdr = value;
            fill_level++;

            if (fill_level > (global_env_size * 3/4)) {
                global_env_rehash();
            }
            return;
        }
        
        next_idx = (next_idx + 1) % global_env_size;

        if (store_type == MODIFY) {
            if (next_idx != start_idx) {
                log_err("Key not found!");
                return;
            }
        }

        check(next_idx != start_idx, "Fatal, no free slots found in global environment!");
    }
    return;

error:
    abort();
}

// Retrieve the value of the global env entry
object_p
local_env_get(env_get_t get_type, object_p lookup_env, object_p key)
{
    debug("Get from local environment");
    object_p parent;
    object_p binding_key;
    int i;

    for (i = 0; i < lookup_env->env.length; i++) {
        binding_key = lookup_env->env.bindings[i].car;

        if (binding_key == key) {
            if (get_type == ENTRY) {
                // for whole entry (cons cell)
                return (object_p)&(lookup_env->env.bindings[i]);
            }
            // only for the value
            return lookup_env->env.bindings[i].cdr;
        }
        // if it is a child env search the parent
        if ((parent = lookup_env->env.parent) != NULL)
            return env_get(get_type, parent, key);

        log_err("Key not found!");

        if (get_type == ENTRY)
            return nil_object;
    }
    return NULL;
}

void
local_env_store(env_store_t store_type, object_p lookup_env, object_p key, object_p value)
{
    debug("Store into local environment");
    object_p binding_key;
    int i;

    for (i = 0; i < lookup_env->env.length; i++) {
        binding_key = lookup_env->env.bindings[i].car;

        if (binding_key == key) {
            lookup_env->env.bindings[i].cdr = value;
            return;
        }

        if (store_type == CREATE) {
            if (binding_key == nil_object) {
                // first free binding
                lookup_env->env.bindings[i].car = key;
                lookup_env->env.bindings[i].cdr = value;
                return;
            }
        }
    }

    if (store_type == MODIFY) {
        object_p parent;

        if ((parent = lookup_env->env.parent) != NULL) {
            env_store(store_type, parent, key, value);
            return;
        }
    }

    log_err("local_env_store %s\n", (store_type == CREATE)
                                        ? "(CREATE): No free slot"
                                        : "(MODIFY): Key not found");
}

object_p
env_get(env_get_t get_type, object_p env, object_p key)
{
    if (env->any.tag == T_GLOBALENV)
        return global_env_get(get_type, key);
    if (env->any.tag == T_LOCALENV)
        return local_env_get(get_type, env, key);

    log_err("env_get: passed env not valid");
    return NULL;
}

void
env_store(env_store_t store_type, object_p env, object_p key, object_p value)
{
    if (env->any.tag == T_GLOBALENV)
        return global_env_store(store_type, key, value);
    if (env->any.tag == T_LOCALENV)
        return local_env_store(store_type, env, key, value);

    log_err("env_get: passed env not valid");
}
