#include <stdlib.h>
#include <stddef.h>

#include "dbg.h"
#include "memory.h"
#include "symbol_table.h"

#define internal static

object_p
alloc_object(object_tag tag, size_t size)
{
    object_p ptr = malloc(size);
    ptr->any.tag = tag;

    return ptr;
}

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

    define_symbol = symbol_table_get_or_put("define");
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
alloc_string(bstring value)
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

object_p
alloc_vector(int length)
{
    struct vector_object *vector;
    int i;

    vector = (struct vector_object *)(malloc(sizeof(struct vector_object)
                + sizeof(object_p) * (length - NUM_DUMMY_ALLOC)));
    vector->tag = T_VECTOR;
    vector->length = length;

    for (i = 0; i < length; i++) {
        vector->elements[i] = nil_object;
    }

    return (object_p)vector;
}

internal object_p
alloc_builtin_object(object_tag tag, char *name, cont_func_t func, int num_params)
{
    struct builtin_object *b_obj;
    b_obj = malloc(sizeof(struct builtin_object));

    b_obj->tag = tag;
    b_obj->name = symbol_table_get_or_put(name);
    b_obj->func = func;
    b_obj->num_params = num_params;

    return (object_p)b_obj;
}

object_p
alloc_builtin_func(char *name, cont_func_t func, int num_params)
{
    object_p builtin_f = alloc_builtin_object(T_BUILTIN_FUNC,\
                                            name, func, num_params);

    return builtin_f;
}

object_p
alloc_builtin_syntax(char *name, cont_func_t func, int num_params)
{
    object_p builtin_s = alloc_builtin_object(T_BUILTIN_SYNTAX,\
                                            name, func, num_params);

    return builtin_s;
}

internal object_p
alloc_userdefined_object(object_tag tag, char *name,\
        object_p param_list, object_p body_list,\
        object_p env)
{
    struct userdefined_object *ud_obj;
    ud_obj = malloc(sizeof(struct userdefined_object));

    ud_obj->tag = tag;
    ud_obj->name = symbol_table_get_or_put(name);
    ud_obj->param_list = param_list;
    ud_obj->body_list = body_list;
    ud_obj->num_params = 0;
    ud_obj->num_locals = 0;
    ud_obj->home_env = env;

    return (object_p)ud_obj;
}

object_p
alloc_userdefined_func(char *name, object_p param_list,\
        object_p body_list, object_p env)
{
    object_p udf = alloc_userdefined_object(T_USERDEFINED_FUNC,
            name, param_list, body_list, env);

    return udf;
}

object_p
alloc_userdefined_syntax(char *name, object_p param_list,\
        object_p body_list, object_p env)
{
    object_p uds = alloc_userdefined_object(T_USERDEFINED_SYNTAX,
            name, param_list, body_list, env);

    return uds;
}

object_p
alloc_scanner(FILE *file)
{
    scanner_t *scanner;

    scanner = (scanner_t *)(malloc(sizeof(scanner_t)));
    scanner->tag = T_SCANNER;
    scanner->file = file;
    scanner->input = bfromcstr("");
    scanner->scan_pos = 0;
    scanner->expr_start = 0;
    scanner->expr_end = 0;
    scanner->eof = 0;
    scanner->nparens_expected = 0;
    scanner->str_pending = 0;
    scanner->pending = 0;

    return (object_p)scanner;
}

object_p
alloc_output_stream(FILE *stream)
{
    object_p out_obj = alloc_object(T_OUTPUT,\
            sizeof(struct output_object));

    out_obj->output.stream = stream;
    out_obj->output.str_buf = NULL;

    return out_obj;
}

object_p
alloc_output_strbuf()
{
    object_p out_obj = alloc_object(T_OUTPUT,\
            sizeof(struct output_object));

    out_obj->output.stream = NULL;
    out_obj->output.str_buf = bfromcstr("");

    return out_obj;
}

cont_p
alloc_continuation()
{
    cont_p continuation;

    continuation = (cont_p)(malloc(sizeof(struct continuation_object)));
    continuation->tag = T_CONTINUATION;
    continuation->caller = NULL;
    continuation->next = NULL;
    continuation->ret_val = NULL;

    return continuation;
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
    int alloc_size = offsetof(struct env_object, bindings) + sizeof(env_binding_t)*length;

    check((parent == NULL ||
            (parent->any.tag == T_LOCALENV ||
             parent->any.tag == T_GLOBALENV)), "Invalid parent environment given!");

    object_p new_env = (object_p)(malloc(alloc_size));

    new_env->env.tag = T_LOCALENV;
    new_env->env.parent = parent;
    new_env->env.length = length;

    for (int i = 0; i < length; i++) {
        new_env->env.bindings[i].tag = T_ENV_BINDING;
        new_env->env.bindings[i].car = nil_object;
        new_env->env.bindings[i].cdr = nil_object;
    }

    return new_env;

error:
    abort();
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
