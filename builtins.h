#ifndef BUILTINS_H
#define BUILTINS_H

#include "memory.h"

cont_p builtin_plus(cont_p cont);
cont_p builtin_plus_selfeval(cont_p cont);
cont_p builtin_minus(cont_p cont);
cont_p builtin_times(cont_p cont);
cont_p builtin_times_selfeval(cont_p cont);
cont_p builtin_if(cont_p cont);
cont_p builtin_if_selfeval(cont_p cont);
cont_p builtin_lambda(cont_p cont);
cont_p builtin_define(cont_p cont);

#endif
