#include <stdarg.h>

#include "logger.h"

int scm_log_level = 0;
output_t *scm_log_output = NULL;

void
scm_log_init(int log_level, output_t *output)
{
    scm_log_level = log_level;
    scm_log_output = output;
}

void
scm_log_printf(int log_level, const char *file, int line, const char *format, ...)
{
    if (log_level < scm_log_level) return;

    char *log_label = NULL;

    switch (log_level) {
        case LOG_DEBUG:
            log_label = "[DEBUG]";
            break;
        case LOG_INFO:
            log_label = "[INFO]";
            break;
        case LOG_WARNING:
            log_label = "[WARNING]";
            break;
        case LOG_ERROR:
            log_label = "[ERROR]";
            break;
        default:
            log_label = NULL;
            break;
    }

    va_list args;
    va_start(args, format);

    if (!scm_log_output) {
        fprintf(stderr, "%s (%s:%d): ", log_label, file, line);
        vfprintf(stderr, format, args);
        fprintf(stderr, "\n");
    } else {
        scm_out_printf(scm_log_output, "%s (%s:%d): ", log_label, file, line);
        scm_out_vprintf(scm_log_output, format, args);
        scm_out_printf(scm_log_output, "\n");
    }

    va_end(args);
}
