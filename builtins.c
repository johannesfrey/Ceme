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
builtin_divide(cont_p cont)
{
    object_p arg_list, first_arg;
    int quotient;
    
    arg_list = cont->args_locals[0];
    scm_check(IS_CONS(arg_list), "(/): Needs at least "
                                "one argument"); 

    first_arg = CAR(arg_list);
    scm_check(IS_NUMBER(first_arg), "(/): Argument "
                                "is not a number");

    quotient = NUMBER_VAL(first_arg);
    arg_list = CDR(arg_list);

    // one element only -> (/ 1 first_arg)
    if (IS_NIL(arg_list)) {
        quotient = 1 / NUMBER_VAL(first_arg);
        CP_RETURN(cont, alloc_number(quotient));
    }

    while (!IS_NIL(arg_list)) {
        object_p num_obj = CAR(arg_list);
        scm_check(IS_NUMBER(num_obj), "(/): Argument "
                                    "is not a number");

        quotient /= NUMBER_VAL(num_obj);
        arg_list = CDR(arg_list);
    }

    CP_RETURN(cont, alloc_number(quotient));

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

cont_p
builtin_car(cont_p cont)
{
    object_p arg_list = cont->args_locals[0];
    object_p arg = CAR(arg_list);

    scm_check(IS_CONS(arg), "(car): Argument "
                            "is not a cons");

    CP_RETURN(cont, CAR(arg));
    
error:
    longjmp(error_occured, 1);
}

cont_p
builtin_cdr(cont_p cont)
{
    object_p arg_list = cont->args_locals[0];
    object_p arg = CAR(arg_list);

    scm_check(IS_CONS(arg), "(cdr): Argument "
                            "is not a cons");

    CP_RETURN(cont, CDR(arg));
    
error:
    longjmp(error_occured, 1);
}

cont_p
builtin_setcar(cont_p cont)
{
    object_p arg_list, new_car, cons;

    arg_list = cont->args_locals[0];
    cons = CAR(arg_list);
    new_car = CAR(CDR(arg_list));

    scm_check(IS_CONS(cons), "[set-car!]: Argument "
                            "is not a cons");

    SET_CAR(cons, new_car);

    CP_RETURN(cont, void_object);

error:
    longjmp(error_occured, 1);
}

cont_p
builtin_setcdr(cont_p cont)
{
    object_p arg_list, new_cdr, cons;

    arg_list = cont->args_locals[0];
    cons = CAR(arg_list);
    new_cdr = CAR(CDR(arg_list));

    scm_check(IS_CONS(cons), "[set-cdr!]: Argument "
                            "is not a cons");

    SET_CDR(cons, new_cdr);

    CP_RETURN(cont, void_object);

error:
    longjmp(error_occured, 1);
}

cont_p
builtin_is_eq(cont_p cont)
{
    object_p arg_list, first_arg, second_arg, ret_val;

    arg_list = cont->args_locals[0];
    first_arg = CAR(arg_list);
    second_arg = CAR(CDR(arg_list));
    ret_val = false_object;

    if (first_arg == second_arg) ret_val = true_object;

    if (IS_NUMBER(first_arg))
        if (IS_NUMBER(second_arg))
            if (NUMBER_VAL(first_arg) == NUMBER_VAL(second_arg))
                ret_val = true_object;

    CP_RETURN(cont, ret_val);
}

cont_p
builtin_is_eq_nr(cont_p cont)
{
    object_p arg_list, first_arg, second_arg, ret_val;

    arg_list = cont->args_locals[0];
    first_arg = CAR(arg_list);
    second_arg = CAR(CDR(arg_list));
    ret_val = false_object;

    scm_check(IS_NUMBER(first_arg),\
            "(=): First argument is not a number");
    scm_check(IS_NUMBER(second_arg),\
            "(=): Second argument is not a number");

    ret_val = (NUMBER_VAL(first_arg) == NUMBER_VAL(second_arg))
        ? true_object
        : false_object;

    CP_RETURN(cont, ret_val);

error:
    longjmp(error_occured, 1);
}

cont_p
builtin_is_lt_nr(cont_p cont)
{
    object_p arg_list, first_arg, second_arg, ret_val;

    arg_list = cont->args_locals[0];
    first_arg = CAR(arg_list);
    second_arg = CAR(CDR(arg_list));
    ret_val = false_object;

    scm_check(IS_NUMBER(first_arg),\
            "(=): First argument is not a number");
    scm_check(IS_NUMBER(second_arg),\
            "(=): Second argument is not a number");

    ret_val = (NUMBER_VAL(first_arg) < NUMBER_VAL(second_arg))
        ? true_object
        : false_object;

    CP_RETURN(cont, ret_val);

error:
    longjmp(error_occured, 1);
}

cont_p
builtin_is_number(cont_p cont)
{
    object_p arg_list, arg;

    arg_list = cont->args_locals[0];
    arg = CAR(arg_list);

    object_p ret_val = IS_NUMBER(arg) ? true_object : false_object;

    CP_RETURN(cont, ret_val);
}

cont_p
builtin_is_symbol(cont_p cont)
{
    object_p arg_list, arg;

    arg_list = cont->args_locals[0];
    arg = CAR(arg_list);

    object_p ret_val = IS_SYMBOL(arg) ? true_object : false_object;

    CP_RETURN(cont, ret_val);
}

cont_p
builtin_is_string(cont_p cont)
{
    object_p arg_list, arg;

    arg_list = cont->args_locals[0];
    arg = CAR(arg_list);

    object_p ret_val = IS_STRING(arg) ? true_object : false_object;

    CP_RETURN(cont, ret_val);
}

cont_p
builtin_is_cons(cont_p cont)
{
    object_p arg_list, arg;

    arg_list = cont->args_locals[0];
    arg = CAR(arg_list);

    object_p ret_val = IS_CONS(arg) ? true_object : false_object;

    CP_RETURN(cont, ret_val);
}

cont_p
builtin_is_function(cont_p cont)
{
    object_p arg_list, arg;

    arg_list = cont->args_locals[0];
    arg = CAR(arg_list);

    object_p ret_val = IS_FUNC(arg) ? true_object : false_object;

    CP_RETURN(cont, ret_val);
}

cont_p
builtin_is_syntax(cont_p cont)
{
    object_p arg_list, arg;

    arg_list = cont->args_locals[0];
    arg = CAR(arg_list);

    object_p ret_val = IS_SYNTAX(arg) ? true_object : false_object;

    CP_RETURN(cont, ret_val);
}

cont_p
builtin_is_builtin_function(cont_p cont)
{
    object_p arg_list, arg;

    arg_list = cont->args_locals[0];
    arg = CAR(arg_list);

    object_p ret_val = IS_BUILTIN_FUNC(arg) ? true_object : false_object;

    CP_RETURN(cont, ret_val);
}

cont_p
builtin_is_builtin_syntax(cont_p cont)
{
    object_p arg_list, arg;

    arg_list = cont->args_locals[0];
    arg = CAR(arg_list);

    object_p ret_val = IS_BUILTIN_SYNTAX(arg) ? true_object : false_object;

    CP_RETURN(cont, ret_val);
}

cont_p
builtin_is_binding(cont_p cont)
{
    object_p arg_list, arg, env_entry, ret_val;

    arg_list = cont->args_locals[0];
    arg = CAR(arg_list);

    scm_check(IS_SYMBOL(arg), "[builtin_is_binding]: Non-symbol received");

    env_entry = env_get_entry(global_env, arg);
    ret_val = IS_NIL(env_entry) ? false_object : true_object;

    CP_RETURN(cont, ret_val);

error:
    longjmp(error_occured, 1);
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
        
        new_udf = alloc_userdefined_func("anonymous lambda", param_list,\
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

            new_udf = alloc_userdefined_func(SYMBOL_VAL(name), param_list,\
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

    else_expr = CAR(rest_list);

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
builtin_quote(cont_p cont)
{
    object_p arg_list = cont->args_locals[1];

    CP_RETURN(cont, CAR(arg_list));
}

internal cont_p builtin_set2(cont_p cont);

cont_p
builtin_set(cont_p cont)
{
    #define env		cont->args_locals[0]
    object_p arg_list, expr;

    arg_list = cont->args_locals[1];

    #define var_name    cont->args_locals[1]
    
    var_name = CAR(arg_list);
    scm_check(IS_SYMBOL(var_name), "[set!]: Argument "
                        "is not a symbol");
            
    arg_list = CDR(arg_list);
    expr = CAR(arg_list);
    arg_list = CDR(arg_list);

    if (IS_NIL(expr)) {
        env_set(env, var_name, expr);
        CP_RETURN(cont, void_object);
    }

    CP_CALL2(cont, scm_eval, env, expr, builtin_set2);

error:
    longjmp(error_occured, 1);
}

cont_p
builtin_set2(cont_p cont)
{
    object_p evaled_expr = cont->ret_val;

    env_set(env, var_name, evaled_expr);

    CP_RETURN(cont, void_object);

    #undef var_name
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
