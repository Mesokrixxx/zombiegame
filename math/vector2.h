#ifndef UTIL_VECTOR2_H
# define UTIL_VECTOR2_H

# include "util/types.h"

// vectorType, vectorPrefix 
# define _VECTOR2_XLIST(X) \
	X(f32, ) \
	X(i32, i)

# define X(type, prefix) \
	typedef struct { type x, y; } V2##prefix; \
	V2##prefix v2##prefix(type x, type y); \
	V2##prefix v2##prefix##_add(V2##prefix v2##prefix##_1, V2##prefix v2##prefix##_2); \
	V2##prefix v2##prefix##_sub(V2##prefix v2##prefix##_1, V2##prefix v2##prefix##_2); \
	V2##prefix v2##prefix##_mul(V2##prefix v2##prefix##_1, V2##prefix v2##prefix##_2); \
	V2##prefix v2##prefix##_div(V2##prefix v2##prefix##_1, V2##prefix v2##prefix##_2); \
	V2##prefix v2##prefix##_max(V2##prefix v2##prefix##_1, V2##prefix v2##prefix##_2); \
	V2##prefix v2##prefix##_min(V2##prefix v2##prefix##_1, V2##prefix v2##prefix##_2);
_VECTOR2_XLIST(X)
# undef X

#endif // UTIL_VECTOR2_H
