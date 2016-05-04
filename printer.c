#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

void print(object *obj)
{
  switch (obj->u.any.type) {
    case T_NUMBER:
      printf("%lld", obj->u.number.value);
      break;
    case T_STRING:
      printf("\"%s\"", obj->u.string.value);
      break;
    default:
      fprintf(stderr, "unimplemented\n");
      exit(1);
  }
}

