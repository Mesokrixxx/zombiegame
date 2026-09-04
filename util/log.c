#include "log.h"

void _vlog_impl(FILE *stream, const char *file, i32 line, const char *func, const char *header, const char *fmt, va_list ap) {
	fprintf(stream, "[%s:%d][%s][%s] ", file, line, func, header);
	
	va_list ap_cpy;

	va_copy(ap_cpy, ap);
	i32 buflen = vsnprintf(NULL, 0, fmt, ap_cpy);
	va_end(ap_cpy);

	char buf[buflen + 1];

	vsnprintf(buf, buflen + 1, fmt, ap);
	fprintf(stream, "%s%s", buf, buf[buflen - 1] != '\n' ? "\n" : "");
}

void _log_impl(FILE *stream, const char *file, i32 line, const char *func, const char *header, const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	_vlog_impl(stream, file, line, func, header, fmt, ap);
	va_end(ap);
}
