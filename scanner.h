#ifndef SCANNER_H
#define SCANNER_H

#include "memory.h"

#define internal static

typedef int (*scan_check_func_t)(int);

void scm_scan_move_forward(scanner_t *scanner, size_t amount);
void scm_scan_move_backward(scanner_t *scanner, size_t amount);

char scm_scan_next_char(scanner_t *scanner);
char smc_scan_prev_char(scanner_t *scanner);

void scm_scan_skip_until(scanner_t *scanner, char c);
void scm_scan_skip_while(scanner_t *scanner, scan_check_func_t cf);

bstring scm_scan_until(scanner_t *scanner, char *delims);
bstring scm_scan_while(scanner_t *scanner, scan_check_func_t cf);

int scm_scan_input(scanner_t *scanner);
int scm_scan_expression(scanner_t *scanner);

#define scm_scan_is_end(A)  ((scm_scan_pos(A) >= scm_scan_length(A) || (A)->eof) ? 1 : 0)
#define scm_scan_length(A)  ((A)->input->slen-1)
#define scm_scan_pos(A)     ((A)->scan_pos)

internal inline char
scm_scan_current_char(scanner_t *scanner)
{
    if (scanner->eof) return EOF;
    if (scanner->scan_pos == scm_scan_length(scanner))
        scanner->eof = 1;
    else
        scanner->eof = 0;

    return scanner->input->data[scanner->scan_pos];
}

#endif
