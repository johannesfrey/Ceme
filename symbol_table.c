#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "dbg.h"
#include "symbol_table.h"
#include "memory.h"

#define internal static

const int SYMBOL_TABLE_INITIAL_SIZE = 511;
uint32_t SuperFastHash(const char *data, int len);

static object_p *symbol_table;
static int symbol_table_size = SYMBOL_TABLE_INITIAL_SIZE;
static int fill_level = 0;

internal uint32_t 
hash(const char *chars)
{
    return SuperFastHash(chars, sizeof(chars)); 
}

internal void
rehash_symbol_table()
{
    int old_size = symbol_table_size;
    int new_size = (old_size + 1) * 2 - 1;
    int idx_old_table;

    object_p *old_table = symbol_table;
    object_p *new_table;

    new_table = (object_p *)(malloc(sizeof(object_p) * new_size));
    memset((void *)new_table, 0, (sizeof(object_p) * new_size));

    for (idx_old_table = 0; idx_old_table < old_size; idx_old_table++) {
        object_p old_symbol = old_table[idx_old_table];

        // hash object for new table and try to insert into next free slot
        if (old_symbol != NULL) {
            uint32_t new_hash = hash(old_symbol->symbol.value);
            int start_idx = new_hash % new_size;
            int next_idx = start_idx;
            object_p peek;

            for (;;) {
                peek = new_table[next_idx];
                
                if (peek == NULL) {
                    new_table[next_idx] = old_symbol;
                    break;
                }

                // slot is occupied; try next
                next_idx = (next_idx + 1) % new_size;

                // looped through table but no free slots found!
                check(next_idx != start_idx, "Fatal, no free slots found in symbol table!");
            }
        }
    }

    free(old_table);
    symbol_table = new_table;
    symbol_table_size = new_size;
    return;

error:
    abort();
}

object_p
symbol_table_get_or_set(char *key)
{
    uint32_t hash_key = hash(key);
    int start_idx = hash_key % symbol_table_size;
    int next_idx;
    object_p peek;

    next_idx = start_idx;

    for (;;) {
        peek = symbol_table[next_idx];

        // SET part of function
        if (peek == NULL) {
            object_p new_sym = alloc_symbol(key);
            symbol_table[next_idx] = new_sym;
            debug("Symbol '%s' added\n", key);
            fill_level++;

            if (fill_level > (symbol_table_size * 3 / 4)) {
                rehash_symbol_table();
            }

            return new_sym;
        }
        
        // GET part of function 
        if (strcmp(key, peek->symbol.value) == 0) {
            debug("Symbol '%s' retrieved\n", key);
            return peek;
        }

        next_idx = (next_idx + 1) % symbol_table_size;

        check(next_idx != start_idx, "Fatal, no free slots found in symbol table!");
    }

error:
    abort();
} 

void 
init_symbol_table()
{
    debug("Initializing symbol table");

    symbol_table_size = SYMBOL_TABLE_INITIAL_SIZE;
    symbol_table = (object_p *)(malloc(sizeof(object_p) * symbol_table_size));
    memset((void *)symbol_table, 0, (sizeof(object_p) * symbol_table_size));
}
