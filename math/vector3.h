#ifndef MATH_VECTOR3_H
# define MATH_VECTOR3_H

# include "util/types.h"

typedef struct {
	f32 x, y, z;
} V3;

V3 v3(f32 x, f32 y, f32 z);

#endif // MATH_VECTOR3_H
