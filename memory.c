#include <stdlib.h>

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
