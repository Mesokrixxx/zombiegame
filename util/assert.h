#ifndef UTIL_ASSERT_H
# define UTIL_ASSERT_H

#include "util/types.h"

void _assert_impl(const char *file, i32 line, const char *func, bool cond, const char *expr, const char *fmt, ...);

# define ASSERT(_c, fmt, ...) \
	_assert_impl(__FILE__, __LINE__, __func__, (_c), #_c, fmt, ##__VA_ARGS__)

# define UNREACHABLE() ASSERT(false, "unreachable")

#endif // UTIL_ASSERT_H
