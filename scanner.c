#include <assert.h>

#include "dbg.h"
#include "scanner.h"

void
scm_scan_move_forward(scanner_t *scanner, size_t amount)
{
    int len = scm_scan_length(scanner);
    size_t new_pos = scanner->scan_pos + amount;

    if (new_pos > len) new_pos = len;

    scanner->scan_pos = new_pos;
}

void
scm_scan_move_backward(scanner_t *scanner, size_t amount)
{
    size_t new_pos = scanner->scan_pos;

    if (new_pos % amount == new_pos) {
        // amount is bigger than the possible steps to
        // go back; clamp to start point
        amount = new_pos;
    }

    new_pos -= amount;

    scanner->scan_pos = new_pos;
}

char
scm_scan_next_char(scanner_t *scanner)
{
    char ch = scm_scan_current_char(scanner);

    if (ch != EOF) scm_scan_move_forward(scanner, 1);

    return ch;
}

char
scm_scan_prev_char(scanner_t *scanner)
{
    scm_scan_move_backward(scanner, 1);

    return scm_scan_current_char(scanner);
}

void
scm_scan_skip_until(scanner_t *scanner, char ch)
{
    if (scanner->eof) return;

    char next;

    do {
        next = scm_scan_next_char(scanner);
    } while (next != ch && next != EOF);

    scm_scan_move_backward(scanner, 1);
}

void
scm_scan_skip_while(scanner_t *scanner, scan_check_func_t cf)
{
    if (scanner->eof) return;

    int rc;
    char next;

    do {
        rc = cf((next = scm_scan_next_char(scanner)));
    } while (rc && next != EOF);

    scm_scan_move_backward(scanner, 1);
}

internal int
set_slice_len(void *parm, int ofs, int len)
{
    *(int *)parm = len;

    return -1;
}

bstring
scm_scan_until(scanner_t *scanner, char *delims)
{
    int slice_len = 0;
    bstring bdelims = bfromcstr(delims);

    bsplitscb(scanner->input, bdelims,\
            scanner->scan_pos, set_slice_len, &slice_len);
    bstring slice = bmidstr(scanner->input,\
            scanner->scan_pos, slice_len);

    if (slice_len > 0) scm_scan_move_forward(scanner, slice_len);

    bdestroy(bdelims);

    return slice;
}

bstring
scm_scan_while(scanner_t *scanner, scan_check_func_t cf)
{
    int slice_len = 0;
    int pos = scanner->scan_pos;
    char next;

    do {
        next = scm_scan_next_char(scanner);
        if (next != EOF) slice_len++;
    } while (cf(next) && next != EOF);

    scm_scan_move_backward(scanner, 1);

    bstring slice = bmidstr(scanner->input, pos, slice_len);

    return slice;
}

internal inline void
reset_scanner_after_complete_expr(scanner_t *scanner)
{
    scanner->scan_pos = scanner->expr_start;
    scanner->expr_start = scanner->expr_end;
    scanner->nparens_expected = 0;
    scanner->str_pending = 0;
    scanner->pending = 0;
    scanner->eof = 0;
}

internal int
check_if_expr_complete(scanner_t *scanner)
{
    // start were we left off
    scanner->scan_pos = scanner->expr_end;

    int rc = 0;
    char c;

    while (!scm_scan_is_end(scanner)) {
        c = scm_scan_next_char(scanner);

        switch (c) {
            case '"':
                if (!scanner->str_pending)
                    scanner->str_pending = 1;
                else
                    scanner->str_pending = 0;
                break;
            case '(':
                if (!scanner->str_pending)
                    scanner->nparens_expected++;
                break;
            case ')':
                if (!scanner->str_pending)
                    scanner->nparens_expected--;
                break;
            default:
                break;
        }
    }

    // save last position for later
    scanner->expr_end = scanner->scan_pos;

    if (!scanner->str_pending && scanner->nparens_expected < 1) {
        reset_scanner_after_complete_expr(scanner);
        rc = 1;
    } else {
        scanner->pending = 1;
    }

    return rc;
}

int
scm_scan_expression(scanner_t *scanner)
{
    int rc;

    rc = bgetsa(scanner->input, (bNgetc) fgetc, scanner->file, '\n');
    check(rc != BSTR_ERR, "Problems reading line from stream");
    rc = check_if_expr_complete(scanner);

    return rc;

error:
    return -1;
}

int
scm_scan_input(scanner_t *scanner)
{
    int rc;

    rc = breada(scanner->input, (bNread)fread, scanner->file);
    check(rc != BSTR_ERR, "Problems reading from stream");

    return rc;

error:
    return -1;
}
