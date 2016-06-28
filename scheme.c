#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "reader.h"
#include "eval.h"
#include "printer.h"
#include "symbol_table.h"

int 
main()
{
    init_symbol_table();
    init_wellknown_objects();

    printf("Welcome to Scheme.\n");

    for (;;) {
        printf("> ");
        print(eval(read_object(stdin)));
        printf("\n");
    }

    return 0;
}

