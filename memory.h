#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

//
// Object structures
//

typedef enum {
    T_NUMBER,
    T_STRING,
    T_SYMBOL,
    T_CONS,
    T_NIL,
    T_VOID,
    T_TRUE,
    T_FALSE,
    T_EOF
} object_tag;

typedef union object *object_p;

struct any_object {
    object_tag tag;
};

struct number_object {
    object_tag tag;
    int64_t value;
};

struct string_object {
    object_tag tag;
    char *value;
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

typedef union object {
    struct any_object any;
    struct number_object number;
    struct string_object string;
    struct symbol_object symbol;
    struct cons_object cons;
} object;

//
// Functions
//

void init_wellknown_objects();

// 
// Allocator functions
//

object_p alloc_number(int64_t value);
object_p alloc_string(char *value);
object_p alloc_symbol(char *value);
object_p alloc_cons(object_p car, object_p cdr);
object_p* alloc_symbol_table(int size);

//
// Well-known objects

object_p nil_object;
object_p void_object;
object_p true_object;
object_p false_object;
object_p eof_object;

#endif
