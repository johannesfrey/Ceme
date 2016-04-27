#include <stdlib.h>

#include "memory.h"

object* alloc_number(int64_t value)
{
  number_object *num_obj;
  num_obj = (number_object *)(malloc(sizeof(number_object)));
  num_obj->type = T_NUMBER;
  num_obj->value = value;
  return (object *)num_obj;
}

