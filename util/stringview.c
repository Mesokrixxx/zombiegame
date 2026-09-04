#include "stringview.h"
#include <string.h>
#include <ctype.h>

StringView stringview(const char *cstr) {
	return (StringView){ .str = cstr, .len = strlen(cstr) };
}

StringView stringview_getLine(const char *line) {
	StringView sv = {
		.str = line,
	};

	while (sv.str[sv.len] && sv.str[sv.len] != '\n')
		sv.len++;
	return sv;
}

StringView stringview_rtrim(StringView sv) {
	u64 off = 0;

	while (isspace(sv.str[off]))
		off++;
	sv.str += off;
	sv.len -= off;
	return sv;
}

StringView stringview_ltrim(StringView sv) {
	while (sv.len && isspace(sv.str[sv.len - 1]))
		sv.len--;
	return sv;
}

StringView stringview_trim(StringView sv) {
	return stringview_rtrim(stringview_ltrim(sv));
}

bool stringview_eq(StringView sv1, StringView sv2) {
	return sv1.len == sv2.len && !strncmp(sv1.str, sv2.str, sv1.len);
}
