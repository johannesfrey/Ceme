#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "reader.h"
#include "eval.h"
#include "printer.h"
#include "symbol_table.h"
#include "environment.h"

void fill_initial_env();
void repl(FILE *input);

int 
main()
{
    init_symbol_table();
    init_wellknown_objects();
    init_global_env();

    fill_initial_env();

    printf("Welcome to Scheme.\n");
    repl(stdin);

    return 0;
}

void
repl(FILE *input)
{
    for (;;) {
        object_p obj;

        printf("> ");
        obj = read_object(input);
        obj = eval(global_env, obj);

        if (obj->any.tag != T_VOID) {
            print(obj);
            printf("\n");
        }
    }
}

void
fill_initial_env()
{
    global_env_store(CREATE, symbol_table_get_or_put("abc"), alloc_number(100));
}
