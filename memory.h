#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

//
// Object structures
//

typedef enum {
  T_NUMBER
} object_type;

struct any_object {
  object_type type;
};

struct number_object {
  object_type type;
  int64_t value;
};

typedef struct object {
  union {
    struct any_object any;
    struct number_object number;
  } u;
} object;

// 
// Allocator functions
//

object* alloc_number(int64_t value);

#endif
