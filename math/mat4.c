#include "mat4.h"

void mat4_identity(Mat4 *mat) {
	*mat = (Mat4){0};

	mat->m0[0] = 1;
	mat->m1[1] = 1;
	mat->m2[2] = 1;
	mat->m3[3] = 1;
}

void mat4_translate(Mat4 *mat, V3 v) {
	mat->m3[0] = mat->m0[0] * v.x + mat->m1[0] * v.y + mat->m2[0] * v.z + mat->m3[0]; 
	mat->m3[1] = mat->m0[1] * v.x + mat->m1[1] * v.y + mat->m2[1] * v.z + mat->m3[1]; 
	mat->m3[2] = mat->m0[2] * v.x + mat->m1[2] * v.y + mat->m2[2] * v.z + mat->m3[2]; 
	mat->m3[3] = mat->m0[3] * v.x + mat->m1[3] * v.y + mat->m2[3] * v.z + mat->m3[3]; 
}

void mat4_ortho(Mat4 *mat, V2 width, V2 height, V2 depth) {
	*mat = (Mat4){0};

	f32 rl = width.y - width.x;
	f32 tb = height.y - height.x;
	f32 fn = depth.y - depth.x;

	mat->m0[0] = 2 / rl;
	mat->m1[1] = 2 / tb;
	mat->m2[2] = -2 / fn;
	mat->m3[3] = 1;
	
	mat->m3[0] = -(width.x + width.y) / rl;
	mat->m3[1] = -(height.x + height.y) / tb;
	mat->m3[2] = -(depth.x + depth.y) / fn;
}
