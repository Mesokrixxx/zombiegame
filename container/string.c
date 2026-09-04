#include "string.h"
#include <string.h>
#include <stdio.h>

String string_create(Allocator *allocator, const char *cstr, u64 reserve) {
	String str;
	u64 cstrLen = cstr ? strlen(cstr) : 0;

	dynlist_init(allocator, str, (cstrLen + 1 < reserve) ? reserve : cstrLen + 1);
	strncpy(str, cstr, cstrLen);
	string_setLength(str, cstrLen);
	str[cstrLen] = 0;
	return str;
}

void string_clear(String str) {
	str[0] = 0;
	dynlist_clear(str);
}

void string_setLength(String str, u64 len) {
	_dynlist_getHeader(str)->used = len;
}

void string_vassign(String str, const char *fmt, va_list ap) {
	va_list ap_copy;

	va_copy(ap_copy, ap);
	int len = vsnprintf(NULL, 0, fmt, ap_copy);
	va_end(ap_copy);

	string_reserve(str, len + 1);
	vsnprintf(str, len + 1, fmt, ap);
	string_setLength(str, len);
}

void string_assign(String str, const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	string_vassign(str, fmt, ap);
	va_end(ap);
}

void string_vappend(String str, const char *fmt, va_list ap) {
	va_list ap_copy;

	va_copy(ap_copy, ap);
	int len = vsnprintf(NULL, 0, fmt, ap_copy);
	va_end(ap_copy);

	u64 strLen = string_length(str);

	string_reserve(str, strLen + len + 1);
	vsnprintf(str + strLen, len + 1, fmt, ap);
	string_setLength(str, strLen + len);
}

void string_append(String str, const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	string_vappend(str, fmt, ap);
	va_end(ap);
}
