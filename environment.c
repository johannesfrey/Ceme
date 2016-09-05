#include <stdlib.h>
#include <assert.h>

#include "scheme.h"
#include "logger.h"
#include "memory.h"
#include "environment.h"

#define internal static

#define ENVIRONMENT_INITIAL_SIZE 511

object_p global_env;
static int global_env_size = ENVIRONMENT_INITIAL_SIZE;
static int fill_level = 0;

typedef enum {
    ENTRY,
    VALUE
} env_get_t; 

typedef enum {
    CREATE,
    MODIFY
} env_store_t; 

void
init_global_env()
{
    scm_debug("Initializing global environment");
    global_env_size = ENVIRONMENT_INITIAL_SIZE;
    global_env = alloc_global_env(global_env_size);
    memset((void*)(&(global_env->env.bindings)), 0, (sizeof(env_binding_t)*global_env_size));
} 

internal void
global_env_rehash()
{
    scm_debug("Rehash global environment");
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
                assert(next_idx != start_idx && \
                    "[global_env_rehash]: Fatal, no free slots "
                    "found in global environment!");
            }
        }
    }

    free(old_env);
    global_env = new_env;
    global_env_size = new_size;
}

internal object_p global_env_get(env_get_t get_type, object_p key);
internal void global_env_store(env_store_t store_type, object_p key, object_p value);
internal object_p local_env_get(env_get_t get_type, object_p lookup_env, object_p key);
internal void local_env_store(env_store_t store_type, object_p lookup_env, object_p key, object_p value);

internal object_p
env_get(env_get_t get_type, object_p env, object_p key)
{
    assert(env != NULL && "[env_get]: Passed environment must not be NULL!");

    scm_check(IS_ENV(env), "[env_get]: Passed environment is invalid!");

    if (IS_GLOBALENV(env))
        return global_env_get(get_type, key);
    if (IS_LOCALENV(env))
        return local_env_get(get_type, env, key);

    return NULL;

error:
    longjmp(error_occured, 1);
}

internal void
env_store(env_store_t store_type, object_p env, object_p key, object_p value)
{
    assert(env != NULL && "[env_store]: Passed environment must not be NULL!");

    scm_check(IS_ENV(env), "[env_store]: Passed environment is invalid!");

    if (IS_GLOBALENV(env))
        return global_env_store(store_type, key, value);
    if (IS_LOCALENV(env))
        return local_env_store(store_type, env, key, value);

error:
    longjmp(error_occured, 1);
}

// Retrieve the value of the global env entry
internal object_p
global_env_get(env_get_t get_type, object_p key)
{
    scm_debug("Get from global environment");
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
        assert(next_idx != start_idx && \
            "[global_env_rehash]: Fatal, no free slots "
            "found in global environment!");
    }
}

internal void
global_env_store(env_store_t store_type, object_p key, object_p value)
{
    scm_debug("Store into global environment");
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
            scm_check(store_type == CREATE, "[global_env_set]: Key not found");

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
            scm_check(next_idx == start_idx, "[global_env_set]: Key not found");
        }

        assert(next_idx != start_idx && \
            "[global_env_rehash]: Fatal, no free slots "
            "found in global environment!");
    }

    return;

error:
    longjmp(error_occured, 1);
}

// Retrieve the value of the global env entry
internal object_p
local_env_get(env_get_t get_type, object_p lookup_env, object_p key)
{
    scm_debug("Get from local environment");
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
    }
    // if it is a child env search the parent
    if ((parent = lookup_env->env.parent) != NULL)
        return env_get(get_type, parent, key);

    scm_log_err("[local_env_get]: Key not found");

    if (get_type == ENTRY)
        return nil_object;

    return NULL;
}

internal void
local_env_store(env_store_t store_type, object_p lookup_env, object_p key, object_p value)
{
    assert(lookup_env->env.bindings != NULL &&
            "[local_env_store]: There must be preallocated bindings");

    scm_debug("Store into local environment");
    object_p binding_key;
    int i;

    for (i = 0; i < lookup_env->env.length; i++) {
        binding_key = lookup_env->env.bindings[i].car;
        assert(binding_key != NULL && "[local_env_store]: Binding must be preallocated!");

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

        parent = lookup_env->env.parent;
        scm_check(parent != NULL, "[local_env_store]: Cannot look beyond global env for key.");

        env_store(store_type, parent, key, value);
        return;
    }

    // fallthrough
    scm_log_err("local_env_store %s\n", (store_type == CREATE)
                                        ? "(CREATE): No free slot"
                                        : "(MODIFY): Key not found");
error:
    longjmp(error_occured, 1);
}

// Convenient API wrappers

object_p
env_get_entry(object_p env, object_p key)
{
    return env_get(ENTRY, env, key);
}

object_p
env_get_value(object_p env, object_p key)
{
    return env_get(VALUE, env, key);
}

void
env_set(object_p env, object_p key, object_p value)
{
    env_store(MODIFY, env, key, value);
}

void
env_put(object_p env, object_p key, object_p value)
{
    env_store(CREATE, env, key, value);
}

object_p
global_env_get_entry(object_p key)
{
    return global_env_get(ENTRY, key);
}

object_p
global_env_get_value(object_p key)
{
    return global_env_get(VALUE, key);
}

void
global_env_set(object_p key, object_p value)
{
    global_env_store(MODIFY, key, value);
}

void
global_env_put(object_p key, object_p value)
{
    global_env_store(CREATE, key, value);
}

object_p
local_env_get_entry(object_p lookup_env, object_p key)
{
    return local_env_get(ENTRY, lookup_env, key);
}

object_p
local_env_get_value(object_p lookup_env, object_p key)
{
    return local_env_get(VALUE, lookup_env, key);
}

void
local_env_set(object_p lookup_env, object_p key, object_p value)
{
    local_env_store(MODIFY, lookup_env, key, value);
}

void
local_env_put(object_p lookup_env, object_p key, object_p value)
{
    local_env_store(CREATE, lookup_env, key, value);
}
