#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "memory.h"

extern const int SYMBOL_TABLE_INITIAL_SIZE;

void init_symbol_table();
object_p symbol_table_get_or_put(char *key);

#endif
