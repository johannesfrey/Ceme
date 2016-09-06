#include <stdio.h>
#include <stdlib.h>

#include "scheme.h"
#include "memory.h"
#include "continuation_passing.h"
#include "scanner.h"
#include "reader.h"
#include "eval.h"
#include "printer.h"
#include "output.h"
#include "symbol_table.h"
#include "environment.h"
#include "builtins.h"

#define internal static

jmp_buf error_occured;
cont_list *continuation_list;

internal void init_cont_list();
internal void fill_initial_env();
internal cont_p repl(cont_p cont);
internal cont_p repl2(cont_p cont);
internal cont_p repl3(cont_p cont);
internal cont_p finish(cont_p cont);
internal void trampoline(cont_p cont);

int 
main()
{
    cont_p cont;
    cont_p caller_cont;
    object_p scanner, output;

    init_symbol_table();
    init_wellknown_objects();
    init_global_env();
    init_cont_list();

    fill_initial_env();

    scanner = alloc_scanner(stdin);
    output = alloc_output_stream(stdout);

    if (setjmp(error_occured) != 0) {
        // reset continuation list
        continuation_list->next = continuation_list->first;
    }

    // initial continuation
    caller_cont = cont_list_obtain(continuation_list);
    caller_cont->args_locals[0] = output;
    caller_cont->next = finish;

    cont = cont_list_obtain(continuation_list);
    cont->caller = caller_cont;
    cont->args_locals[0] = scanner;
    cont->args_locals[1] = output;
    cont->next = repl;

    trampoline(cont);

    return 0;
}

internal cont_p
repl(cont_p cont)
{
    int complete;
    object_p scan_obj = cont->args_locals[0];
    object_p out_obj = cont->args_locals[1];

    do {
        scm_print_object(&out_obj->output, scan_obj);
        complete = scm_scan_expression(&scan_obj->scanner);
    } while (!complete);

    CP_TAILCALL2(cont, repl2, scan_obj, out_obj);
}

internal cont_p
repl2(cont_p cont)
{
    object_p scan_obj = cont->args_locals[0];
    object_p obj;

    obj = read_object(&scan_obj->scanner);

    CP_CALL2(cont, scm_eval, global_env, obj, repl3);
}

internal cont_p
repl3(cont_p cont)
{
    object_p obj = cont->ret_val;
    object_p scan_obj = cont->args_locals[0];
    object_p out_obj = cont->args_locals[1];

    if (TAG(obj) != T_VOID) {
        scm_print_object(&out_obj->output, obj);
        scm_out_printf(&out_obj->output, "\n");
    }

    if (!scm_scan_is_end(&scan_obj->scanner))
        CP_TAILCALL2(cont, repl2, scan_obj, out_obj);

    CP_TAILCALL2(cont, repl, scan_obj, out_obj);
}

internal void
trampoline(cont_p cont)
{
    cont_p next_cont = cont;
    cont_func_t func;

    for (;;) {
        func = next_cont->next;
        next_cont = (*func)(next_cont);
    }
}

internal cont_p
finish(cont_p cont) {
    object_p out_obj = cont->args_locals[0];

    scm_print_object(&out_obj->output, cont->ret_val);

    exit(0);
}

internal void
init_cont_list()
{
    continuation_list = cont_list_create();
    cont_list_init(continuation_list);
}

internal void
fill_initial_env()
{
    global_env_put(symbol_table_get_or_put("+"), alloc_builtin_func("+", builtin_plus, -1));
    global_env_put(symbol_table_get_or_put("*"), alloc_builtin_func("*", builtin_times, -1));
    global_env_put(symbol_table_get_or_put("-"), alloc_builtin_func("-", builtin_minus, -1));

    global_env_put(symbol_table_get_or_put("eq?"), alloc_builtin_func("eq?", builtin_is_eq, 2));
    global_env_put(symbol_table_get_or_put("="), alloc_builtin_func("=", builtin_is_eq_nr, 2));
    global_env_put(symbol_table_get_or_put("<"), alloc_builtin_func("<", builtin_is_lt_nr, 2));

    global_env_put(symbol_table_get_or_put("number?"), alloc_builtin_func("number?", builtin_is_number, 1));
    global_env_put(symbol_table_get_or_put("symbol?"), alloc_builtin_func("symbol?", builtin_is_symbol, 1));
    global_env_put(symbol_table_get_or_put("string?"), alloc_builtin_func("string?", builtin_is_string, 1));
    global_env_put(symbol_table_get_or_put("cons?"), alloc_builtin_func("cons?", builtin_is_cons, 1));
    global_env_put(symbol_table_get_or_put("function?"), alloc_builtin_func("function?", builtin_is_function, 1));
    global_env_put(symbol_table_get_or_put("syntax?"), alloc_builtin_func("syntax?", builtin_is_syntax, 1));
    global_env_put(symbol_table_get_or_put("builtin-function?"), alloc_builtin_func("builtin-function?", builtin_is_builtin_function, 1));
    global_env_put(symbol_table_get_or_put("builtin-syntax?"), alloc_builtin_func("builtin-syntax?", builtin_is_builtin_syntax, 1));
    global_env_put(symbol_table_get_or_put("binding?"), alloc_builtin_func("binding?", builtin_is_binding, 1));

    global_env_put(symbol_table_get_or_put("cons"), alloc_builtin_func("cons", builtin_cons, 2));
    global_env_put(symbol_table_get_or_put("car"), alloc_builtin_func("car", builtin_car, 1));
    global_env_put(symbol_table_get_or_put("cdr"), alloc_builtin_func("cdr", builtin_cdr, 1));

    global_env_put(symbol_table_get_or_put("define"), alloc_builtin_syntax("define", builtin_define, -1));
    global_env_put(symbol_table_get_or_put("lambda"), alloc_builtin_syntax("lambda", builtin_lambda, -1));
    global_env_put(symbol_table_get_or_put("if"), alloc_builtin_syntax("if", builtin_if, 3));
    global_env_put(symbol_table_get_or_put("quote"), alloc_builtin_syntax("quote", builtin_quote, 1));
}
