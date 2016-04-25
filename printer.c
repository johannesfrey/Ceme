#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

void print(object *obj)
{
  switch (obj->type)
  {
    case T_NUM:
      printf("%lld", obj->u.num.num_val);
      break;
    default:
      fprintf(stderr, "unimplemented\n");
      exit(1);
  }
}

