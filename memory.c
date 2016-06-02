#include <stdlib.h>

#include "memory.h"

void init_wellknown_objects()
{
  nil_object = (struct object *)(malloc(sizeof(struct any_object)));
  nil_object->u.any.type = T_NIL;

  void_object = (struct object *)(malloc(sizeof(struct any_object)));
  void_object->u.any.type = T_VOID;
  
  true_object = (struct object *)(malloc(sizeof(struct any_object)));
  true_object->u.any.type = T_TRUE;
  
  false_object = (struct object *)(malloc(sizeof(struct any_object)));
  false_object->u.any.type = T_FALSE;
  
  eof_object = (struct object *)(malloc(sizeof(struct any_object)));
  eof_object->u.any.type = T_EOF;
}

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
