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
    print(eval(read(stdin)));
    printf("\n");
  }

  return 0;
}

