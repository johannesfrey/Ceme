#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../dbg.h"
#include "../memory.h"
#include "../symbol_table.h"

int test_symbol_table()
{
    object_p s1, s2, s3;

    s1 = symbol_table_get_or_set("alpha");
    check(s1->u.any.tag == T_SYMBOL, "Wrong tag for symbol");
    check((strcmp(s1->u.symbol.value, "alpha") == 0), "Wrong value in symbol");
    
    s2 = symbol_table_get_or_set("beta");
    check(s1->u.any.tag == T_SYMBOL, "Wrong tag for symbol");
    check((strcmp(s2->u.symbol.value, "beta") == 0), "Wrong value in symbol");

    s3 = symbol_table_get_or_set("alpha");
    check(s1->u.any.tag == T_SYMBOL, "Wrong tag for symbol");
    check((strcmp(s3->u.symbol.value, "alpha") == 0), "Wrong value in symbol");

    check(s1 == s3, "Not retrieving the right symbol from symbol table");
    check(s1 != s2, "Not distinct symbols even though they should");

    {
        char c1, c2, c3;

        for (c1 = 'a'; c1 <= 'z'; c1++) {
            for (c2 = 'a'; c2 <= 'z'; c2++) {
                for (c3 = 'a'; c3 <= 'z'; c3++) {
                    char sym[4];
                    char *sym_copy;

                    sym[0] = c1;
                    sym[1] = c2;
                    sym[2] = c3;
                    sym[3] = '\0';
                    sym_copy = malloc(4);
                    strcpy(sym_copy, sym);
                    symbol_table_get_or_set(sym_copy);
                }
            }
        }
    }

    return 0;

error:
    return -1;
}

int main()
{
    init_symbol_table();
    check(test_symbol_table() == 0, "test_symbol_table failed\n");

    return 0;

error:
    return -1;

}
