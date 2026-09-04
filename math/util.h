#ifndef MATH_UTIL_H
# define MATH_UTIL_H

# include "util/types.h"

static inline u64 math_roundupPow2(u64 n, u64 p) {
	return (n + (p - 1)) & ~(p - 1);
}

#endif // MATH_UTIL_H
