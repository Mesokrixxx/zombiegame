#ifndef MATH_VECTOR4_H
# define MATH_VECTOR4_H

# include "util/types.h"
# include "math/vector2.h"

typedef union {
	struct { f32 x, y, z, w; };
	struct { f32 r, g, b, a; };
} V4;

V4 v4(f32 x, f32 y, f32 z, f32 w);
V4 v4fv2s(V2 v2_1, V2 v2_2);

#endif // MATH_VECTOR4_H
