#include "output.h"

int
scm_out_printf(output_t *output, const char *format, ...)
{
    int rslt;
    va_list args;

    va_start(args, format);
    rslt = scm_out_vprintf(output, format, args);
    va_end(args);

    return rslt;
}

int
scm_out_vprintf(output_t *output, const char *format, va_list args)
{
    int rslt;

    if (output->stream)
        rslt = vfprintf(output->stream, format, args);
    else
        rslt = bformata(output->str_buf, format, args);

    return rslt;
}
