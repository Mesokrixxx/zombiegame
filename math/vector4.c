#include "vector4.h"

V4 v4(f32 x, f32 y, f32 z, f32 w) {
	return (V4){{x, y, z, w}};
}

V4 v4fv2s(V2 v2_1, V2 v2_2) {
	return v4(v2_1.x, v2_1.y, v2_2.x, v2_2.y);
}

