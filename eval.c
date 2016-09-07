#include <stdlib.h>
#include <assert.h>

#include "scheme.h"
#include "logger.h"
#include "dbg.h"
#include "memory.h"
#include "continuation_passing.h"
#include "environment.h"

#define internal static

internal cont_p eval_cons(cont_p cont);
internal cont_p eval_cons2(cont_p cont);
internal cont_p eval_builtin_or_userdefined(cont_p cont);

internal cont_p eval_builtin(cont_p cont);
internal cont_p eval_builtin_func(cont_p cont);
internal cont_p eval_builtin_func2(cont_p cont);
internal cont_p eval_builtin_func3(cont_p cont);

internal cont_p eval_userdefined(cont_p cont);
internal cont_p eval_userdefined2(cont_p cont);
internal cont_p eval_userdefined3_1(cont_p cont);
internal cont_p eval_userdefined3_2(cont_p cont);
internal cont_p eval_userdefined4(cont_p cont);

internal inline int count_args(object_p arg_list);

cont_p
scm_eval(cont_p cont)
{
    #define env                 cont->args_locals[0]
    #define expr                cont->args_locals[1]

    assert(IS_ENV(env) && "[scm_eval]: Non-environment received");

    // all except symbols and lists eval to themselves
    if (TAG(expr) < T_SELFEVALUATING) {
        CP_RETURN(cont, expr);
    } else if (TAG(expr) == T_SYMBOL) {
        object_p val = env_get_value(env, expr);
        scm_check(val != NULL, "[scm_eval]: Undefined variable: %s", SYMBOL_VAL(expr));

        CP_RETURN(cont, val);
    } else if (TAG(expr) == T_CONS) {
        CP_TAILCALL2(cont, eval_cons, env, expr);
    } else {
        scm_log_err("[scm_eval]: Unknown object type");
    }

// Fallthrough
error:
    longjmp(error_occured, 1);
}

internal cont_p
eval_cons(cont_p cont)
{
    assert(IS_ENV(env) && "[scm_eval]: Non-environment received");

    object_p unevaled_func_slot;

    unevaled_func_slot = CAR(expr);

    CP_CALL2(cont, scm_eval, env, unevaled_func_slot, eval_cons2);
}

internal cont_p
eval_cons2(cont_p cont)
{
    #define evaled_func_slot cont->args_locals[2]

    evaled_func_slot = cont->ret_val;
    scm_check((IS_SYNTAX(evaled_func_slot) || IS_FUNC(evaled_func_slot)),
            "[eval_cons2]: Unexpected object in function slot");

    CP_TAILCALL3(cont, eval_builtin_or_userdefined, env, expr, evaled_func_slot);

error:
    longjmp(error_occured, 1);
}

cont_p
eval_builtin_or_userdefined(cont_p cont)
{
    #define unevaled_arg_list cont->args_locals[3]

    unevaled_arg_list = CDR(expr);

    if (IS_BUILTIN(evaled_func_slot)) {
        CP_JUMP(cont, eval_builtin)
    } else if (IS_USERDEFINED(evaled_func_slot)) {
        CP_JUMP(cont, eval_userdefined)
    } else {
        CP_RETURN(cont, nil_object);
    }
}

internal cont_p
eval_builtin(cont_p cont)
{
    int num_params;

    num_params = evaled_func_slot->builtin.num_params;
    if (num_params != -1) {
        // we are a func or syntax with a specific arg count
        int num_args = count_args(unevaled_arg_list);
        scm_check(num_args != -2, "[eval_builtin]: Invalid argument given");
        scm_check(num_args == num_params, \
                "[eval_builtin]: Argument arity mismatch: "
                "Expected %d, received %d", num_params, num_args);
    }

    if (IS_BUILTIN_FUNC(evaled_func_slot)) {
        // if we are a builtin func we have to eval our args first
        CP_JUMP(cont, eval_builtin_func);
    } else if (IS_BUILTIN_SYNTAX(evaled_func_slot)) {
        // if we are a builtin syntax we pass them over unevaled
        CP_TAILCALL3(cont, evaled_func_slot->builtin.func, env, unevaled_arg_list, evaled_func_slot);
    }

error:
    longjmp(error_occured, 1);
}

internal cont_p
eval_builtin_func(cont_p cont)
{
    #define values_list cont->args_locals[4]
    #define last_value cont->args_locals[5]

    values_list = nil_object;
    last_value = nil_object;

    CP_JUMP(cont, eval_builtin_func2);
}

internal cont_p
eval_builtin_func2(cont_p cont)
{
    if (IS_CONS(unevaled_arg_list)) {
        // if there are unevaled args left eval them first
        object_p unevaled_arg;

        unevaled_arg = CAR(unevaled_arg_list);

        CP_CALL2(cont, scm_eval, env, unevaled_arg, eval_builtin_func3);
    } else {
        // after all args have been evaluated call builtin func
        CP_TAILCALL1(cont, evaled_func_slot->builtin.func, values_list);
    }
}

internal cont_p
eval_builtin_func3(cont_p cont)
{
    // build list of values out of evaled args here
    object_p evaled_arg, new_cons;

    evaled_arg = cont->ret_val;
    new_cons = alloc_cons(evaled_arg, nil_object);

    if (IS_NIL(values_list)) { // starting point
        values_list = new_cons;
    } else {
        scm_check(IS_CONS(last_value),\
                "[eval_builtin_func3]: last_value must be a cons");
        last_value->cons.cdr = new_cons;
    }

    last_value = new_cons;
    unevaled_arg_list = CDR(unevaled_arg_list);

    CP_JUMP(cont, eval_builtin_func2);

    #undef last_value
    #undef values_list
error:
    longjmp(error_occured, 1);
}

internal cont_p
eval_userdefined(cont_p cont)
{
    #define params      cont->args_locals[4]
    #define new_env     cont->args_locals[5]
    #define arg_count   cont->args_locals[6]
    int num_params, num_locals;

    arg_count = alloc_number(0);
    params = evaled_func_slot->userdefined.param_list;
    num_params = evaled_func_slot->userdefined.num_params;
    num_locals = evaled_func_slot->userdefined.num_locals;

    // For userdefined objects create a new env first
    new_env = alloc_local_env(num_params+num_locals, evaled_func_slot->userdefined.home_env);

    CP_JUMP(cont, eval_userdefined2);
}

internal cont_p
eval_userdefined2(cont_p cont)
{
    if (IS_CONS(unevaled_arg_list) || IS_NIL(unevaled_arg_list)) {
        object_p unevaled_arg;

        unevaled_arg = IS_CONS(unevaled_arg_list)\
                       ? CAR(unevaled_arg_list)
                       : unevaled_arg_list;

        if (IS_SYMBOL(params)) {
            // nevermind, varargs
            #define start_var_arg_cons   cont->args_locals[7]
            #define last_var_arg_cons    cont->args_locals[8]

            start_var_arg_cons = nil_object;

            CP_CALL2(cont, scm_eval, env, unevaled_arg, eval_userdefined3_1);
        } else if (IS_CONS(params) || IS_NIL(params)) {
            // pre check the number of given arguments
            int num_params, num_args;

            num_params = evaled_func_slot->userdefined.num_params;
            num_args = count_args(unevaled_arg_list);

            scm_check(num_params == num_args, \ "[eval_userdefined2]: Argument arity mismatch: "
                "Expected %d, received %d", num_params, num_args);

            CP_CALL2(cont, scm_eval, env, unevaled_arg, eval_userdefined3_2);
        }
    }

// fallthrough
error:
    longjmp(error_occured, 1);
}

internal cont_p
eval_userdefined3_1(cont_p cont)
{
    // bind evaled va_args to a single param
    // in the new env of the userdefined object here

    object_p evaled_arg, cur_var_arg;

    evaled_arg = cont->ret_val;
    cur_var_arg = alloc_cons(evaled_arg, nil_object);

    if (IS_NIL(start_var_arg_cons)) // starting point
        start_var_arg_cons = cur_var_arg;
    else
        SET_CDR(last_var_arg_cons, cur_var_arg);

    last_var_arg_cons = cur_var_arg;

    if (IS_CONS(unevaled_arg_list))
        unevaled_arg_list = CDR(unevaled_arg_list);

    if (IS_CONS(unevaled_arg_list)) {
        // we have more args to eval
        object_p unevaled_arg = CAR(unevaled_arg_list);
        CP_CALL2(cont, scm_eval, env, unevaled_arg, eval_userdefined3_1);
    } else { // all args are evaled
        object_p param = params;

        env_put(new_env, param, start_var_arg_cons);

        // In case of a userdefined we have to eval the body list
        expr = evaled_func_slot->userdefined.body_list;
        scm_check(IS_CONS(expr), "[eval_userdefined2]: Empty body in "
                "userdefined: %s", USERDEFINED_NAME(evaled_func_slot));

        // let the eval of the body list happen in eval_userdefined4
        CP_JUMP(cont, eval_userdefined4);
    }

    #undef start_var_arg_cons
    #undef last_var_arg_cons
error:
    longjmp(error_occured, 1);

}

internal cont_p
eval_userdefined3_2(cont_p cont)
{
    // bind evaled arg to corresponding param
    // in the new env of the userdefined object here

    object_p evaled_arg, param;

    evaled_arg = cont->ret_val;
    param = nil_object;

    if (IS_CONS(params)) {
        param = CAR(params);
        params = CDR(params);
    } else if (IS_SYMBOL(params)) {
        param = params;
        params = nil_object;
    }

    if (!IS_NIL(param))
        env_put(new_env, param, evaled_arg);

    if (IS_CONS(unevaled_arg_list))
        unevaled_arg_list = CDR(unevaled_arg_list);

    if (IS_CONS(unevaled_arg_list)) {
        // we have more args to eval
        object_p unevaled_arg = CAR(unevaled_arg_list);
        CP_CALL2(cont, scm_eval, env, unevaled_arg, eval_userdefined3_2);
    } else { // all args are evaled

        // In case of a userdefined we have to eval the body list
        expr = evaled_func_slot->userdefined.body_list;
        scm_check(IS_CONS(expr), "[eval_userdefined2]: Empty body in "
                "userdefined: %s", USERDEFINED_NAME(evaled_func_slot));

        // let the eval of the body list happen in eval_userdefined4
        CP_JUMP(cont, eval_userdefined4);
    }

error:
    longjmp(error_occured, 1);
}

internal cont_p
eval_userdefined4(cont_p cont)
{
    // eval the body list here

    object_p last_value;
    object_p body_expr;

    last_value = cont->ret_val;

    if (IS_USERDEFINED_FUNC(evaled_func_slot)) {
        // expr is our body list
        if (IS_CONS(expr)) {
            body_expr = CAR(expr);
            expr = CDR(expr);

            if (IS_NIL(expr)) // no more expressions to come
                CP_TAILCALL2(cont, scm_eval, new_env, body_expr);

            // else there are still expression left
            CP_CALL2(cont, scm_eval, new_env, body_expr, eval_userdefined4);
        }

        // if we come here finished (expr is nil) we simply return the last value
        CP_RETURN(cont, last_value);
    }

    CP_RETURN(cont, nil_object);

    #undef arg_count
    #undef new_env
    #undef params
    #undef unevaled_arg_list
    #undef evaled_func_slot
    #undef env
    #undef expr
}

internal inline int
count_args(object_p arg_list)
{
    int count = 0;

    while (arg_list != nil_object) {
        if (!IS_CONS(arg_list)) {
            count = -2;
            break;
        } else {
            count++;
            arg_list = CDR(arg_list);
        }
    }

    return count;
}
