#include "vector2.h"
#include "math/macros.h"

#define X(type, prefix) \
	V2##prefix v2##prefix(type x, type y) { return (V2##prefix){ .x = x, .y = y }; } \
	V2##prefix v2##prefix##_add(V2##prefix v2##prefix##_1, V2##prefix v2##prefix##_2) { return v2##prefix(v2##prefix##_1.x + v2##prefix##_2.x, v2##prefix##_1.y + v2##prefix##_2.y); } \
	V2##prefix v2##prefix##_sub(V2##prefix v2##prefix##_1, V2##prefix v2##prefix##_2) { return v2##prefix(v2##prefix##_1.x - v2##prefix##_2.x, v2##prefix##_1.y - v2##prefix##_2.y); } \
	V2##prefix v2##prefix##_mul(V2##prefix v2##prefix##_1, V2##prefix v2##prefix##_2) { return v2##prefix(v2##prefix##_1.x * v2##prefix##_2.x, v2##prefix##_1.y * v2##prefix##_2.y); } \
	V2##prefix v2##prefix##_div(V2##prefix v2##prefix##_1, V2##prefix v2##prefix##_2) { return v2##prefix(v2##prefix##_1.x / v2##prefix##_2.x, v2##prefix##_1.y / v2##prefix##_2.y); } \
	V2##prefix v2##prefix##_max(V2##prefix v2##prefix##_1, V2##prefix v2##prefix##_2) { return v2##prefix(MAX(v2##prefix##_1.x, v2##prefix##_2.x), MAX(v2##prefix##_1.y, v2##prefix##_2.y)); } \
	V2##prefix v2##prefix##_min(V2##prefix v2##prefix##_1, V2##prefix v2##prefix##_2) { return v2##prefix(MIN(v2##prefix##_1.x, v2##prefix##_2.x), MIN(v2##prefix##_1.y, v2##prefix##_2.y)); }
_VECTOR2_XLIST(X)
#undef X
