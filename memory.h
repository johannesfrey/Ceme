#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

//
// Object structures
//

typedef enum {
  T_NUM
} object_type;

struct num_object {
  int64_t num_val;
};

typedef struct object {
  object_type type;
  union {
    struct num_object num;
  } u;
} object;

// 
// Allocator functions
//

object* alloc_num(int64_t value);

#endif
