#include <stdlib.h>

#include "memory.h"

object* alloc_number(int64_t value)
{
  struct number_object *num_obj;
  num_obj = (struct number_object *)(malloc(sizeof(struct number_object)));
  num_obj->type = T_NUMBER;
  num_obj->value = value;
  return (object *)num_obj;
}

object* alloc_string(char *value)
{
  struct string_object *str_obj;
  str_obj = (struct string_object *)(malloc(sizeof(struct string_object)));
  str_obj->type = T_STRING;
  str_obj->value = value;
  return (object *)str_obj;
}
