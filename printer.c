#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "output.h"

#define internal static

internal void print_cons(output_t *output, object_p cons_obj);

void 
scm_print_object(output_t *output, object_p obj)
{
    switch (TAG(obj)) {
        case T_SCANNER:
            if (obj->scanner.pending)
                scm_out_printf(output, "..."); 
            else
                scm_out_printf(output, ">  ");

            for (int i=0;i<=obj->scanner.nparens_expected;i++)
                scm_out_printf(output, " ");
            break;

        case T_NUMBER:
            scm_out_printf(output, "%lld", NUMBER_VAL(obj));
            break;

        case T_CONS:
            if (CAR(obj) == define_symbol && IS_CONS(CDR(obj))) {
                scm_out_printf(output, "'");
                scm_print_object(output, CDR(obj));
            } else {
                print_cons(output, obj);
            }
            break;

        case T_STRING:
            scm_out_printf(output, "\"");

            for(int i=0;i<obj->string.value->slen;i++) {
                char c = obj->string.value->data[i];

                switch (c) {
                    case '\r':
                        scm_out_printf(output, "\\r");
                        break;
                    case '\t':
                        scm_out_printf(output, "\\t");
                        break;
                    case '\n':
                        scm_out_printf(output, "\\n");
                        break;
                    default:
                        scm_out_printf(output, "%c", c);
                        break;
                }
            }

            scm_out_printf(output, "\"");
            break;

        case T_USERDEFINED_FUNC:
            scm_out_printf(output, "<procedure: %s>", USERDEFINED_NAME(obj));
            break;

        case T_SYMBOL:
            scm_out_printf(output, "%s", SYMBOL_VAL(obj));
            break;

        case T_NIL:
            scm_out_printf(output, "nil");
            break;

        case T_TRUE:
            scm_out_printf(output, "#t");
            break;

        case T_FALSE:
            scm_out_printf(output, "#f");
            break;

        default:
            scm_out_printf(output, "unknown object");
            break;
    }
}

internal void
print_cons(output_t *output, object_p cons_obj)
{
    scm_out_printf(output, "(");

    while (IS_CONS(cons_obj)) {
        scm_print_object(output, CAR(cons_obj));
        cons_obj = CDR(cons_obj);

        if (IS_CONS(cons_obj))
            scm_out_printf(output, " ");
    }

    if (!IS_NIL(cons_obj)) {
        scm_out_printf(output, " . ");
        scm_print_object(output, cons_obj);
    }

    scm_out_printf(output, ")");
}
