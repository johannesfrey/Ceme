#ifndef CONTINUATION_PASSING_H
#define CONTINUATION_PASSING_H

#include <assert.h>

#include "memory.h"

//
// Continuation memory management (doubly linked list)
//

#define internal static

#define DEFAULT_EXPAND_RATE 64

typedef struct cont_list_node {
    struct cont_list_node *next;
    struct cont_list_node *prev;
    cont_p cont;
} cont_list_node;

typedef struct cont_list {
    int count;
    cont_list_node *first;
    cont_list_node *last;
    cont_list_node *next;
} cont_list;

extern cont_list *continuation_list;

cont_list *cont_list_create();

#define cont_list_count(L)      ((L)->count)
#define cont_list_first(L)      ((L)->first != NULL ? (L)->first->cont : NULL)
#define cont_list_last(L)       ((L)->last != NULL ? (L)->last->cont : NULL)
#define cont_list_next(L)       ((L)->next != NULL ? (L)->next->cont : NULL)

void cont_list_init(cont_list *list);

void cont_list_push_back(cont_list *list, cont_p cont);
void cont_list_push_front(cont_list *list, cont_p cont);

cont_p cont_list_obtain(cont_list *list);
void cont_list_release(cont_list *list);

cont_p cont_list_remove(cont_list *list, cont_list_node *node);

//
// Continuation passing calls
//

#define CP_CALL1(cont, func, arg1, next) \
    return _CP_CALL1(cont, func, arg1, next);

internal inline cont_p
_CP_CALL1(cont_p cont, cont_func_t func, object_p arg1, cont_func_t next)
{
    assert(continuation_list != NULL && "Continuation list must be preallocated");

    cont_p new_cont = cont_list_obtain(continuation_list);
    cont->next = next;
    new_cont->caller = cont;
    new_cont->args_locals[0] = arg1;
    new_cont->next = func;
    return new_cont;
}

#define CP_CALL2(cont, func, arg1, arg2, next) \
    return _CP_CALL2(cont, func, arg1, arg2, next);

internal inline cont_p
_CP_CALL2(cont_p cont, cont_func_t func, object_p arg1, object_p arg2, cont_func_t next)
{
    assert(continuation_list != NULL && "Continuation list must be preallocated");

    cont_p new_cont = cont_list_obtain(continuation_list);
    cont->next = next; 
    new_cont->caller = cont; 
    new_cont->args_locals[0] = arg1; 
    new_cont->args_locals[1] = arg2; 
    new_cont->next = func; 
    return new_cont;
}

#define CP_CALL3(cont, func, arg1, arg2, arg3, next) \
    return _CP_CALL3(cont, func, arg1, arg2, arg3, next);

internal inline cont_p
_CP_CALL3(cont_p cont, cont_func_t func, object_p arg1, object_p arg2, object_p arg3, cont_func_t next)
{
    assert(continuation_list != NULL && "Continuation list must be preallocated");

    cont_p new_cont = cont_list_obtain(continuation_list);
    cont->next = next; 
    new_cont->caller = cont; 
    new_cont->args_locals[0] = arg1; 
    new_cont->args_locals[1] = arg2; 
    new_cont->args_locals[2] = arg3; 
    new_cont->next = func; 
    return new_cont;
}

#define CP_CALL4(cont, func, arg1, arg2, arg3, arg4, next) \
    return _CP_CALL4(cont, func, arg1, arg2, arg3, arg4, next);

internal inline cont_p
_CP_CALL4(cont_p cont, cont_func_t func, object_p arg1, object_p arg2, object_p arg3, object_p arg4, cont_func_t next)
{
    assert(continuation_list != NULL && "Continuation list must be preallocated");

    cont_p new_cont = cont_list_obtain(continuation_list);
    cont->next = next; 
    new_cont->caller = cont; 
    new_cont->args_locals[0] = arg1; 
    new_cont->args_locals[1] = arg2; 
    new_cont->args_locals[2] = arg3; 
    new_cont->args_locals[3] = arg4; 
    new_cont->next = func; 
    return new_cont;
}

#define CP_TAILCALL1(cont, func, arg1) \
    return _CP_TAILCALL1(cont, func, arg1);

internal inline cont_p
_CP_TAILCALL1(cont_p cont, cont_func_t func, object_p arg1)
{
    cont->args_locals[0] = arg1;
    cont->next = func;
    return cont;
}

#define CP_TAILCALL2(cont, func, arg1, arg2) \
    return _CP_TAILCALL2(cont, func, arg1, arg2);

internal inline cont_p
_CP_TAILCALL2(cont_p cont, cont_func_t func, object_p arg1, object_p arg2)
{
    cont->args_locals[0] = arg1;
    cont->args_locals[1] = arg2;
    cont->next = func;
    return cont;
}

#define CP_TAILCALL3(cont, func, arg1, arg2, arg3) \
    return _CP_TAILCALL3(cont, func, arg1, arg2, arg3);

internal inline cont_p
_CP_TAILCALL3(cont_p cont, cont_func_t func, object_p arg1, object_p arg2, object_p arg3)
{
    cont->args_locals[0] = arg1;
    cont->args_locals[1] = arg2;
    cont->args_locals[2] = arg3;
    cont->next = func;
    return cont;
}

#define CP_RETURN(cont, ret_val) \
    return _CP_RETURN(cont, ret_val);

internal inline cont_p
_CP_RETURN(cont_p cont, object_p ret_val)
{
    assert(continuation_list != NULL && "Continuation list must be preallocated");

    cont->caller->ret_val = ret_val;
    cont_list_release(continuation_list);
    return cont->caller;
}

#define CP_JUMP(cont, func) \
    return _CP_JUMP(cont, func);

internal inline cont_p
_CP_JUMP(cont_p cont, cont_func_t func)
{
    cont->next = func;
    return cont;
}

#endif
