#include <stdlib.h>

#include "dbg.h"
#include "memory.h"
#include "environment.h"

object_p
eval(object_p env, object_p expr)
{
    switch(expr->any.tag) {
        default:
            return expr;

        case T_SYMBOL:
            {
                object_p val = env_get(VALUE, env, expr);
                check(val != NULL, "undefined variable: %s", expr->symbol.value);

                return val;
            }
    }
    return NULL;

error:
    abort();
}
