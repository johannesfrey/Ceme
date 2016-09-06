#include <stdlib.h>
#include <stddef.h>

#include "dbg.h"
#include "memory.h"
#include "symbol_table.h"

#define internal static

internal object_p
alloc_object(object_tag tag, size_t size)
{
    object_p ptr = malloc(size);
    ptr->any.tag = tag;

    return ptr;
}

void 
init_wellknown_objects()
{
    size_t any_obj_size = sizeof(struct any_object);

    nil_object = alloc_object(T_NIL, any_obj_size);
    void_object = alloc_object(T_VOID, any_obj_size);
    true_object = alloc_object(T_TRUE, any_obj_size);
    false_object = alloc_object(T_FALSE, any_obj_size);
    eof_object = alloc_object(T_EOF, any_obj_size);

    define_symbol = symbol_table_get_or_put("define");
    quote_symbol = symbol_table_get_or_put("quote");
}

object_p 
alloc_number(int64_t value)
{
    object_p num_obj = alloc_object(T_NUMBER,\
            sizeof(struct number_object));

    num_obj->number.value = value;

    return num_obj;
}

object_p 
alloc_string(bstring value)
{
    object_p str_obj = alloc_object(T_STRING,\
            sizeof(struct string_object));

    str_obj->string.value = value;

    return str_obj;
}

object_p 
alloc_symbol(char *value)
{
    object_p sym_obj = alloc_object(T_SYMBOL,\
            sizeof(struct symbol_object));

    sym_obj->symbol.value = value;

    return sym_obj;
}

object_p 
alloc_cons(object_p car, object_p cdr)
{
    if (car == NULL || cdr == NULL) {
        log_warn("Trying to allocate cons with NULL pointer as member");

        return nil_object;
    }

    {
        object_p cons_obj = alloc_object(T_CONS,\
                sizeof(struct cons_object));

        cons_obj->cons.car = car;
        cons_obj->cons.cdr = cdr;

        return cons_obj;
    }
}

object_p
alloc_vector(int length)
{
    size_t vec_size;
    int i;

    vec_size = sizeof(struct vector_object) \
        + sizeof(object_p) * (length - NUM_DUMMY_ALLOC);

    object_p vec_obj = alloc_object(T_VECTOR, vec_size);

    vec_obj->vector.length = length;

    for (i = 0; i < length; i++) {
        vec_obj->vector.elements[i] = nil_object;
    }

    return vec_obj;
}

internal object_p
alloc_builtin_object(object_tag tag, char *name, cont_func_t func, int num_params)
{
    object_p bi_obj = alloc_object(tag, sizeof(struct builtin_object));

    bi_obj->builtin.name = symbol_table_get_or_put(name);
    bi_obj->builtin.func = func;
    bi_obj->builtin.num_params = num_params;

    return bi_obj;
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
    object_p ud_obj = alloc_object(tag, sizeof(struct userdefined_object));

    ud_obj->userdefined.name = symbol_table_get_or_put(name);
    ud_obj->userdefined.param_list = param_list;
    ud_obj->userdefined.body_list = body_list;
    ud_obj->userdefined.num_params = 0;
    ud_obj->userdefined.num_locals = 0;
    ud_obj->userdefined.home_env = env;

    return ud_obj;
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
    object_p scan_obj = alloc_object(T_SCANNER,\
            sizeof(scanner_t));

    scan_obj->scanner.file = file;
    scan_obj->scanner.input = bfromcstr("");
    scan_obj->scanner.scan_pos = 0;
    scan_obj->scanner.expr_start = 0;
    scan_obj->scanner.expr_end = 0;
    scan_obj->scanner.eof = 0;
    scan_obj->scanner.nparens_expected = 0;
    scan_obj->scanner.str_pending = 0;
    scan_obj->scanner.pending = 0;

    return scan_obj;
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
    cont_p cont = malloc(sizeof(struct continuation_object));

    cont->tag = T_CONTINUATION;
    cont->caller = NULL;
    cont->next = NULL;
    cont->ret_val = NULL;

    return cont;
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
