#include "scheme.h"
#include "logger.h"
#include "builtins.h"
#include "eval.h"
#include "environment.h"
#include "continuation_passing.h"

#define internal static

internal int count_defines(object_p body_list);
internal int count_params(object_p params_list);

cont_p
builtin_plus(cont_p cont)
{
    object_p arg_list = cont->args_locals[0];
    int sum = 0;

    while (!IS_NIL(arg_list)) {
        object_p num_obj = CAR(arg_list);
        scm_check(IS_NUMBER(num_obj), "(+): Argument "
                                    "is not a number");

        sum += NUMBER_VAL(num_obj);
        arg_list = CDR(arg_list);
    }

    CP_RETURN(cont, alloc_number(sum));

error:
    longjmp(error_occured, 1);
}

cont_p
builtin_times(cont_p cont)
{
    object_p arg_list = cont->args_locals[0];
    int product = 1;

    while (!IS_NIL(arg_list)) {
        object_p num_obj = CAR(arg_list);
        scm_check(IS_NUMBER(num_obj), "(*): Argument "
                                    "is not a number");

        product *= NUMBER_VAL(num_obj);
        arg_list = CDR(arg_list);
    }

    CP_RETURN(cont, alloc_number(product));

error:
    longjmp(error_occured, 1);
}

cont_p
builtin_minus(cont_p cont)
{
    object_p arg_list = cont->args_locals[0];
    int diff = 0;
    object_p first_arg;

    scm_check(IS_CONS(arg_list), "(-): Needs at least "
                                "one argument"); 

    first_arg = CAR(arg_list);
    scm_check(IS_NUMBER(first_arg), "(-): Argument "
                                "is not a number");

    diff = NUMBER_VAL(first_arg);
    arg_list = CDR(arg_list);

    // one element only; make it negative
    if (IS_NIL(arg_list))
        CP_RETURN(cont, alloc_number(-diff));

    while (!IS_NIL(arg_list)) {
        object_p next_arg;

        next_arg = CAR(arg_list);
        scm_check(IS_NUMBER(next_arg), "(-): Argument "
                                    "is not a number");

        diff -= NUMBER_VAL(next_arg);
        arg_list = CDR(arg_list);
    }

    CP_RETURN(cont, alloc_number(diff));

error:
    longjmp(error_occured, 1);
}

cont_p
builtin_cons(cont_p cont)
{
    object_p arg_list = cont->args_locals[0];
    object_p first_arg = CAR(arg_list);
    object_p second_arg = CAR(CDR(arg_list));

    CP_RETURN(cont, alloc_cons(first_arg, second_arg));
}

cont_p builtin_if2(cont_p cont);

cont_p
builtin_if(cont_p cont)
{
    #define env                 cont->args_locals[0]
    #define arg_list            cont->args_locals[1]
    #define if_expr             cont->args_locals[2]
    #define else_expr           cont->args_locals[3]

    object_p rest_list;
    object_p cond_expr;

    cond_expr = CAR(arg_list);
    rest_list = CDR(arg_list);

    if_expr = CAR(rest_list);
    rest_list = CDR(rest_list);

    CP_CALL2(cont, scm_eval, env, cond_expr, builtin_if2);
}

cont_p
builtin_if2(cont_p cont)
{
    object_p evaled_cond = cont->ret_val;

    object_p expr_to_eval;

    if (evaled_cond == false_object)
        expr_to_eval = else_expr;
    else
        expr_to_eval = if_expr;

    CP_TAILCALL2(cont, scm_eval, env, expr_to_eval);

    #undef else_expr
    #undef if_expr
    #undef arg_list
    #undef env
}

cont_p
builtin_lambda(cont_p cont)
{
    object_p env = cont->args_locals[0];
    object_p arg_list = cont->args_locals[1];

    scm_check(IS_CONS(arg_list), "(lambda): Needs at least 2 arguments. "
                                    "No arguments given.");

    object_p param_list = CAR(arg_list);
    object_p body_list = CDR(arg_list);

    scm_check(IS_CONS(body_list), "(lambda): Needs at least 2 arguments. "
                                    "No second argument given.");

    scm_check((param_list == nil_object
                || IS_CONS(param_list)
                || IS_SYMBOL(param_list)), "(lambda): Invalid parameter declared.");

    {
        object_p new_udf;
        int num_params;
        
        new_udf = alloc_userdefined_func("anon lambda", param_list,\
                                            body_list, env);
        num_params = count_params(param_list);
        scm_check(num_params >= 0, "(lambda): Invalid element in parameter list");

        new_udf->userdefined.num_params = num_params;
        new_udf->userdefined.num_locals = count_defines(body_list);
        
        CP_RETURN(cont, new_udf);
    }

error:
    longjmp(error_occured, 1);
}

cont_p builtin_define2(cont_p);

cont_p
builtin_define(cont_p cont)
{
    #define env         cont->args_locals[0]
    #define arg_list    cont->args_locals[1]

    scm_check(IS_CONS(arg_list), "(define): Needs at least 2 arguments. "
                                    "No arguments given.");

    #define first_arg   CAR(arg_list)
    object_p rest_args = CDR(arg_list);

    scm_check(IS_CONS(rest_args), "(define): Needs at least 2 arguments. "
                                    "No second argument given.");

    // case: (define var expr)
    if (IS_SYMBOL(first_arg)) {
        // if there are more than 2 args, only the last one is considered
        while (!IS_NIL(CDR(rest_args))) {
            rest_args = CDR(rest_args);
        }
        CP_CALL2(cont, scm_eval, env, CAR(rest_args), builtin_define2);
    }

    // case: (define (name arg_list) body_list)
    if (IS_CONS(first_arg)) {
        object_p name = CAR(first_arg);

        if (IS_SYMBOL(name)) {
            object_p param_list = CDR(first_arg);
            object_p body_list = rest_args;
            object_p new_udf;
            int num_params;

            new_udf = alloc_userdefined_func("anon lambda", param_list,\
                                                body_list, env);
            num_params = count_params(param_list);
            scm_check(num_params >= 0, "(define): Invalid element in parameter list");

            new_udf->userdefined.num_params = num_params;
            new_udf->userdefined.num_locals = count_defines(body_list);
            env_put(env, name, new_udf);

            CP_RETURN(cont, void_object);
        }
    }

    // fallthrough
    scm_log_err("(define): Syntax error");

error:
    longjmp(error_occured, 1);
}

cont_p
builtin_define2(cont_p cont)
{
    #define name    first_arg
    object_p evaled_expr = cont->ret_val;

    env_put(env, name, evaled_expr);

    CP_RETURN(cont, void_object);

    #undef name
    #undef first_arg
    #undef arg_list
    #undef env
}

internal int
is_define(object_p expr)
{
    int rc = 0;

    if (TAG(expr) == T_CONS) {
        if (CAR(expr) == define_symbol)
            rc = 1;
    }

    return rc;
}

internal int
count_params(object_p params_list)
{
    int count = 0;

    while (params_list != nil_object) {
        if (!IS_CONS(params_list)) {
            if (!IS_SYMBOL(params_list)) {
                count = -1;
                break;
            }
            ++count;
            break;
        }
        count++;
        params_list = CDR(params_list);
    }

    return count;
}

internal int
count_defines(object_p body_list)
{
    int count = 0;

    while (body_list != nil_object) {
        if (is_define(CAR(body_list)))
            count++;

        body_list = CDR(body_list);
    }

    return count;
}
