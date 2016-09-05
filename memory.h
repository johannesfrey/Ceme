#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdint.h>

#include "bstrlib.h"

#define NUM_DUMMY_ALLOC 1

//
// Object structures
//

typedef enum {
    T_VOID,
    T_NIL,
    T_TRUE,
    T_FALSE,
    T_EOF,
    T_NUMBER,
    T_STRING,
    T_VECTOR,
    T_CONTINUATION,
    T_GLOBALENV,
    T_LOCALENV,
    T_BUILTIN_SYNTAX,
    T_BUILTIN_FUNC,
    T_USERDEFINED_SYNTAX,
    T_USERDEFINED_FUNC,
    T_SCANNER,
    T_OUTPUT,
    // above are all self evaluating
    T_SELFEVALUATING,

    T_CONS,
    T_SYMBOL,
} object_tag;

typedef union object *object_p;
typedef struct continuation_object *cont_p;
typedef struct scanner_object scanner_t;
typedef struct output_object output_t;

struct any_object {
    object_tag tag;
};

typedef cont_p (*cont_func_t)();

struct continuation_object {
    object_tag tag;
    cont_p caller;
    cont_func_t next;
    object_p ret_val;
    object_p args_locals[10];
};

struct number_object {
    object_tag tag;
    int64_t value;
};

struct string_object {
    object_tag tag;
    bstring value;
};

struct symbol_object {
    object_tag tag;
    char *value;
};

struct cons_object {
    object_tag tag;
    object_p car;
    object_p cdr;
};

struct vector_object {
    object_tag tag;
    int length;
    object_p elements[NUM_DUMMY_ALLOC];
};

struct userdefined_func_object {
    object_tag tag;
    object_p internal_name;
    object_p arg_list;
    object_p body_list;
    object_p home_env;
};

struct builtin_object {
    object_tag tag;
    object_p name;
    int num_params;
    cont_func_t func;
};

struct userdefined_object {
    object_tag tag;
    object_p name;
    object_p param_list;
    object_p body_list;
    int num_params;
    int num_locals;
    object_p home_env;
};

struct scanner_object {
    object_tag tag;
    FILE *file;
    bstring input;
    size_t scan_pos;
    size_t expr_start;
    size_t expr_end;
    int eof;
    int nparens_expected;
    int str_pending;
    int pending;
};

struct output_object {
    object_tag tag;
    FILE *stream;
    bstring str_buf;
};

typedef object_p (*object_func_t)();
typedef void* (*void_func_t)();

#define T_ENV_BINDING T_CONS
#define env_binding_object cons_object
typedef struct env_binding_object env_binding_t;

struct env_object {
    object_tag tag;
    int length;
    object_p parent;
    env_binding_t bindings[];
};

typedef union object {
    struct any_object any;
    struct number_object number;
    struct string_object string;
    struct symbol_object symbol;
    struct cons_object cons;
    struct env_object env;
    struct builtin_object builtin;
    struct userdefined_object userdefined;
    struct scanner_object scanner;
    struct output_object output;
    cont_p cont;
} object;

//
// Convenient Macros
//

#define TAG(o)              ((o)->any.tag)
#define CAR(o)              ((o)->cons.car)
#define CDR(o)              ((o)->cons.cdr)
#define SET_CAR(o,v)        ((o)->cons.car = (v))
#define SET_CDR(o,v)        ((o)->cons.cdr = (v))
#define NUMBER_VAL(o)       ((o)->number.value)
#define STRING_VAL(o)       ((o)->string.value->data)
#define SYMBOL_VAL(o)       ((o)->symbol.value)
#define USERDEFINED_NAME(o) ((o)->userdefined.name->symbol.value)

#define IS_CONS(o)                  (TAG(o) == T_CONS)
#define IS_NUMBER(o)                (TAG(o) == T_NUMBER)
#define IS_STRING(o)                (TAG(o) == T_STRING)
#define IS_SYMBOL(o)                (TAG(o) == T_SYMBOL)
#define IS_VOID(o)                  (TAG(o) == T_VOID)
#define IS_NIL(o)                   (TAG(o) == T_NIL)
#define IS_ENV(o)                   (IS_GLOBALENV(o) || IS_LOCALENV(o))
#define IS_GLOBALENV(o)             (TAG(o) == T_GLOBALENV)
#define IS_LOCALENV(o)              (TAG(o) == T_LOCALENV)
#define IS_USERDEFINED_FUNC(o)      (TAG(o) == T_USERDEFINED_FUNC)
#define IS_USERDEFINED_SYNTAX(o)    (TAG(o) == T_USERDEFINED_SYNTAX)
#define IS_BUILTIN_FUNC(o)          (TAG(o) == T_BUILTIN_FUNC)
#define IS_BUILTIN_SYNTAX(o)        (TAG(o) == T_BUILTIN_SYNTAX)
#define IS_FUNC(o)                  (IS_USERDEFINED_FUNC(o) || IS_BUILTIN_FUNC(o))
#define IS_SYNTAX(o)                (IS_USERDEFINED_SYNTAX(o) || IS_BUILTIN_SYNTAX(o))
#define IS_USERDEFINED(o)           (IS_USERDEFINED_FUNC(o) || IS_USERDEFINED_SYNTAX(o))
#define IS_BUILTIN(o)               (IS_BUILTIN_FUNC(o) || IS_BUILTIN_SYNTAX(o))

//
// Functions
//

void init_wellknown_objects();

// 
// Allocator functions
//

object_p alloc_number(int64_t value);
object_p alloc_string(bstring value);
object_p alloc_symbol(char *value);
object_p alloc_cons(object_p car, object_p cdr);
object_p* alloc_symbol_table(int size);
object_p alloc_global_env(int length);
object_p alloc_env(int length, object_p parent);
object_p alloc_global_env(int length);
object_p alloc_vector(int length);
object_p alloc_builtin_func(char *name, cont_func_t func, int num_params);
object_p alloc_builtin_syntax(char *name, cont_func_t func, int num_params);
object_p alloc_userdefined_func(char *name,\
        object_p arg_list, object_p body_list, object_p env);
object_p alloc_userdefined_syntax(char *name,\
        object_p arg_list, object_p body_list, object_p env);
object_p alloc_scanner(FILE *file); 
object_p alloc_output_stream(FILE *stream);
object_p alloc_output_strbuf();
cont_p alloc_continuation();

//
// Globally visible global environment
// (initialized via init_global_env() in environment.c)
//

extern object_p global_env;

//
// Well-known objects
//

object_p nil_object, void_object, true_object, false_object, eof_object;
object_p define_symbol;

#endif
