#include "types.h"
#include <GL/glew.h>

GLTypeInfo gltypes_getInfo(GLType type) {
	const GLTypeInfo infos[] = {
		[GLTYPES_F32] = { .glTarget = GL_FLOAT, .size = sizeof(float) },
		[GLTYPES_U8] = { .glTarget = GL_UNSIGNED_BYTE, .size = sizeof(u8) },
	};

	return infos[type];
}
