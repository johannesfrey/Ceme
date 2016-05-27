#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "reader.h"
#include "eval.h"
#include "printer.h"

static void init_wellknown_objects()
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
  
int main()
{
  init_wellknown_objects();

  printf("Welcome to Scheme.\n");

  for (;;) {
    printf("> ");
    print(eval(read_object(stdin)));
    printf("\n");
  }

  return 0;
}

