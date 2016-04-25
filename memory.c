#include <stdlib.h>

#include "memory.h"

object* alloc_object()
{
  object *obj;

  obj = (object *)(malloc(sizeof(object)));
  return obj;
}

object* alloc_num(int64_t value)
{
  object *obj = alloc_object();
  obj->type = T_NUM;
  obj->u.num.num_val = value;
  return obj;
}

