#include "assert.h"
#include "util/log.h"
#include <stdlib.h>

void _assert_impl(const char *file, i32 line, const char *func, bool cond, const char *expr, const char *fmt, ...) {
	if (cond)
		return ;

	ERROR("assertion failed '%s'", expr);
	
	va_list ap;

	va_start(ap, fmt);
	_vlog_impl(stderr, file, line, func, ANSI_RED "ERR" ANSI_RST, fmt, ap);
	va_end(ap);
	
	exit(1);
}
