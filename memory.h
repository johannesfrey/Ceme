#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

//
// Object structures
//

typedef enum {
  T_NUMBER,
  T_STRING,
  T_NIL,
  T_VOID,
  T_TRUE,
  T_FALSE,
  T_EOF
} object_type;

struct any_object {
  object_type type;
};

struct number_object {
  object_type type;
  int64_t value;
};

struct string_object {
  object_type type;
  char *value;
};

typedef struct object {
  union {
    struct any_object any;
    struct number_object number;
    struct string_object string;
  } u;
} object;

// 
// Allocator functions
//

object* alloc_number(int64_t value);
object* alloc_string(char *value);

//
// Well-known objects

object* nil_object;
object* void_object;
object* true_object;
object* false_object;
object* eof_object;

#endif
