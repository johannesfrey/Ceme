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
    T_NIL,
    T_VOID,
    T_TRUE,
    T_FALSE,
    T_EOF
} object_tag;

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

typedef struct object {
    union {
        struct any_object any;
        struct number_object number;
        struct string_object string;
        struct symbol_object symbol;
    } u;
} object, *object_p;

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

//
// Well-known objects

object_p nil_object;
object_p void_object;
object_p true_object;
object_p false_object;
object_p eof_object;

#endif
