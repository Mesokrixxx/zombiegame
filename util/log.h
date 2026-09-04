#ifndef UTIL_LOG_H
# define UTIL_LOG_H

# include "util/types.h"
# include <stdio.h>
# include <stdarg.h>

void _vlog_impl(FILE *stream, const char *file, i32 line, const char *func, const char *header, const char *fmt, va_list ap);
void _log_impl(FILE *stream, const char *file, i32 line, const char *func, const char *header, const char *fmt, ...);

# define ANSI_RST "\e[0m"
# define ANSI_RED "\e[31m"
# define ANSI_YEL "\e[33m"
# define ANSI_BLU "\e[34m"

# define LOG(fmt, ...) _log_impl(stdout, __FILE__, __LINE__, __func__, ANSI_BLU "LOG" ANSI_RST, fmt, ##__VA_ARGS__)
# define WARN(fmt, ...) _log_impl(stdout, __FILE__, __LINE__, __func__, ANSI_YEL "WARN" ANSI_RST, fmt, ##__VA_ARGS__)
# define ERROR(fmt, ...) _log_impl(stderr, __FILE__, __LINE__, __func__, ANSI_RED "ERROR" ANSI_RST, fmt, ##__VA_ARGS__)

#endif // UTIL_LOG_H
