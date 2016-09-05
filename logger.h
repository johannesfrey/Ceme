#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "output.h"

#define LOG_DEBUG   1
#define LOG_INFO    2
#define LOG_WARNING 3
#define LOG_ERROR   4

void scm_log_init(int log_level, output_t *output);
void scm_log_printf(int log_level, const char *file, int line, const char *format, ...);

#ifdef DEBUG
#define scm_debug(...) scm_log_printf(LOG_DEBUG, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define scm_debug(...)
#endif

#define scm_log_err(...) scm_log_printf(LOG_ERROR, __FILE__, __LINE__, ##__VA_ARGS__)
#define scm_log_warn(...) scm_log_printf(LOG_WARNING, __FILE__, __LINE__, ##__VA_ARGS__)
#define scm_log_info(...) scm_log_printf(LOG_INFO, __FILE__, __LINE__, ##__VA_ARGS__)

#define scm_check(A, ...) if(!(A)) { scm_log_err(__VA_ARGS__); errno=0; goto error; }
#define scm_sentinel(...) { scm_log_err(__VA_ARGS__); errno=0; goto error; }

#endif
