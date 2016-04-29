#include <stdio.h>

#include "reader.h"
#include "eval.h"
#include "printer.h"

int main()
{
  printf("Welcome to Scheme.\n");

  for (;;)
  {
    printf("> ");
    print(eval(read_object(stdin)));
    printf("\n");
  }

  return 0;
}

