#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "dbg.h"
#include "reader.h"
#include "symbol_table.h"

#define internal static

internal object_p read_list(scanner_t *scanner);
internal object_p read_symbol(bstring slice);
internal object_p read_number(bstring slice);
internal int is_all_digit(const_bstring bstr);

object_p 
read_object(scanner_t *scanner)
{
    int ch;

    scm_scan_skip_while(scanner, isspace);
    ch = scm_scan_next_char(scanner);

    if (ch == '(') {
        return read_list(scanner);
    }
    if (ch == '"') {
        bstring slice = scm_scan_until(scanner, "\"");
        scm_scan_move_forward(scanner, 1);
        return alloc_string(slice);
    }
    if (ch == EOF) {
        return nil_object;
    }
    if (ch == '\'') { // quote
        object_p after_quote = read_object(scanner);
        return alloc_cons(symbol_table_get_or_put("quote"),\
                alloc_cons(after_quote, nil_object));
    } else { // digit or symbol
        scm_scan_move_backward(scanner, 1);
        bstring slice = scm_scan_until(scanner, "\"\'(); \n");

        if (slice->slen < 1)
            return nil_object;

        if (is_all_digit(slice))
            return read_number(slice);

        return read_symbol(slice);
    }
}

internal object_p
read_list(scanner_t *scanner)
{
    int ch;

    // Every list corresponds to a cons; init with nil for car and cdr
    object_p start_list = alloc_cons(nil_object, nil_object);
    object_p current = start_list;
    scm_scan_skip_while(scanner, isspace);

    ch = scm_scan_next_char(scanner);

    if (ch == ')')
        return nil_object;

    scm_scan_move_backward(scanner, 1);

    for (;;) {
        current->cons.car = read_object(scanner);
        scm_scan_skip_while(scanner, isspace);

        ch = scm_scan_next_char(scanner);

        if (ch == ')') {
            current->cons.cdr = nil_object;
            break;
        }
        if (ch == EOF) {
            return nil_object;
        }
        scm_scan_move_backward(scanner, 1);

        // Guarantee that the cdr of the current list is also a cons
        // and set it as the current one for the next loop iteration.
        current->cons.cdr = alloc_cons(nil_object, nil_object);
        current = current->cons.cdr;
    }

    return start_list;
}

internal object_p
read_symbol(bstring slice)
{
    if (biseqcstr(slice, "nil")) {
        bdestroy(slice);
        return nil_object;
    }
    if (slice->data[0] == '#') {
        if (biseqcstr(slice, "#t")) {
            bdestroy(slice);
            return true_object;
        }
        if (biseqcstr(slice, "#f")) {
            bdestroy(slice);
            return false_object;
        }
    }

    // Do not free slice here, as alloc_symbol does not
    // alloc the string value by itself

    return symbol_table_get_or_put((char *)slice->data);
}

internal object_p
read_number(bstring slice)
{
    int val;

    // TODO: negative numbers

    val = atoi((char *)slice->data);

    bdestroy(slice);

    return alloc_number(val);
}

internal int
is_all_digit(const_bstring bstr)
{
    int i;

    for (i = 0; i < bstr->slen; i++) {
        if (!isdigit(bstr->data[i]))
            return 0;
    }
    return 1;
}
