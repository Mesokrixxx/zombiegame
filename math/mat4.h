#ifndef MATH_MAT4_H
# define MATH_MAT4_H

# include "util/types.h"
# include "math/vector2.h"
# include "math/vector3.h"

typedef union {
	struct { f32 m0[4], m1[4], m2[4], m3[4]; };
	f32 raw[16];
} Mat4;

void mat4_identity(Mat4 *mat);
void mat4_translate(Mat4 *mat, V3 v);
void mat4_ortho(Mat4 *mat, V2 width, V2 height, V2 depth);

#endif // MATH_MAT4_H
