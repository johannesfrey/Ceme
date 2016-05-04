#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

//
// Object structures
//

typedef enum {
  T_NUMBER,
  T_STRING
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

#endif
