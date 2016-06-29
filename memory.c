#include <stdlib.h>
#include <stddef.h>

#include "dbg.h"
#include "memory.h"

void 
init_wellknown_objects()
{
    nil_object = (object_p)(malloc(sizeof(struct any_object)));
    nil_object->any.tag = T_NIL;

    void_object = (object_p)(malloc(sizeof(struct any_object)));
    void_object->any.tag = T_VOID;

    true_object = (object_p)(malloc(sizeof(struct any_object)));
    true_object->any.tag = T_TRUE;

    false_object = (object_p)(malloc(sizeof(struct any_object)));
    false_object->any.tag = T_FALSE;

    eof_object = (object_p)(malloc(sizeof(struct any_object)));
    eof_object->any.tag = T_EOF;
}

object_p 
alloc_number(int64_t value)
{
    struct number_object *num_obj;
    num_obj = (struct number_object *)(malloc(sizeof(struct number_object)));
    num_obj->tag = T_NUMBER;
    num_obj->value = value;

    return (object_p)num_obj;
}

object_p 
alloc_string(char *value)
{
    struct string_object *str_obj;
    str_obj = (struct string_object *)(malloc(sizeof(struct string_object)));
    str_obj->tag = T_STRING;
    str_obj->value = value;

    return (object_p)str_obj;
}

object_p 
alloc_symbol(char *value)
{
    struct symbol_object *sym_obj;
    sym_obj = (struct symbol_object *)(malloc(sizeof(struct symbol_object)));
    sym_obj->tag = T_SYMBOL;
    sym_obj->value = value;

    return (object_p)sym_obj;
}

object_p 
alloc_cons(object_p car, object_p cdr)
{
    if (car == NULL || cdr == NULL) {
        log_warn("Trying to allocate cons with NULL pointer as member");

        return nil_object;
    }

    {
        struct cons_object *cons_obj;

        cons_obj = (struct cons_object *)(malloc(sizeof(struct cons_object)));
        cons_obj->tag = T_CONS;
        cons_obj->car = car;
        cons_obj->cdr = cdr;

        return (object_p)cons_obj;
    }
}

object_p*
alloc_symbol_table(int size)
{
    object_p *symbol_table;

    symbol_table = (object_p *)(malloc(sizeof(object_p) * size));

    return symbol_table;
}

object_p
alloc_env(int length, object_p parent)
{
    // slots above bindings slot in env_object + the number of bindings
    int alloc_size = offsetof(struct env_object, bindings) + sizeof(ENV_BINDING)*length;

    check((parent == NULL ||
            (parent->any.tag == T_LOCALENV ||
             parent->any.tag == T_GLOBALENV)), "Invalid parent environment given!");

    object_p new_env = (object_p)(malloc(alloc_size));
    new_env->env.tag = T_LOCALENV;
    new_env->env.parent = parent;
    new_env->env.length = length;

    for (int i = 0; i < length; i++) {
        new_env->env.bindings[i].car = nil_object;
        new_env->env.bindings[i].cdr = nil_object;
    }

    return new_env;
}

object_p
alloc_global_env(int length)
{
    object_p new_env;

    new_env = alloc_env(length, NULL);
    new_env->env.tag = T_GLOBALENV;
    new_env->env.parent = NULL;

    return new_env;
}
