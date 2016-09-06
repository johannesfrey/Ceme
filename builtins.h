#ifndef BUILTINS_H
#define BUILTINS_H

#include "memory.h"

cont_p builtin_plus(cont_p cont);
cont_p builtin_minus(cont_p cont);
cont_p builtin_times(cont_p cont);
cont_p builtin_cons(cont_p cont);
cont_p builtin_car(cont_p cont);
cont_p builtin_cdr(cont_p cont);
cont_p builtin_is_eq(cont_p cont);
cont_p builtin_is_eq_nr(cont_p cont);
cont_p builtin_is_lt_nr(cont_p cont);
cont_p builtin_is_number(cont_p cont);
cont_p builtin_is_symbol(cont_p cont);
cont_p builtin_is_cons(cont_p cont);

cont_p builtin_lambda(cont_p cont);
cont_p builtin_define(cont_p cont);
cont_p builtin_if(cont_p cont);
cont_p builtin_quote(cont_p cont);

#endif
