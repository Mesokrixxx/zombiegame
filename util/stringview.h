#ifndef UTIL_STRINGVIEW_H
# define UTIL_STRINGVIEW_H

# include "util/types.h"
# include "util/macros.h"

typedef struct {
	const char *str;
	u64 len;
} StringView;

# define STRINGVIEW(cstr) ((StringView){ .str = (cstr), .len = ARRAY_SIZE(cstr) - 1 })
# define STRINGVIEW_FMT(sv) (int)sv.len, sv.str

StringView stringview(const char *cstr);
StringView stringview_getLine(const char *line);
StringView stringview_rtrim(StringView sv);
StringView stringview_ltrim(StringView sv);
StringView stringview_trim(StringView sv);
bool stringview_eq(StringView sv1, StringView sv2);

#endif // UTIL_STRINGVIEW_H
