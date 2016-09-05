#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdio.h>
#include "memory.h"

int scm_out_printf(output_t *output, const char *format, ...);
int scm_out_vprintf(output_t *output, const char *format, va_list args);

#endif
