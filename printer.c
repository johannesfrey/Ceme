#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

void print(object *obj)
{
    switch (obj->u.any.tag) {
        case T_NUMBER:
            printf("%lld", obj->u.number.value);
            break;
        case T_STRING:
            printf("\"%s\"", obj->u.string.value);
            break;
        case T_NIL:
            printf("nil");
            break;
        case T_TRUE:
            printf("#t");
            break;
        case T_FALSE:
            printf("#f");
            break;
        default:
            fprintf(stderr, "unimplemented\n");
            exit(1);
    }
}

