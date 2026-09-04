#include "draw.h"
#include <GL/glew.h>

static u32 getGlDrawMethod(GLDrawMethod method) {
	const u32 glDMethods[] = {
		[GLDRAW_METHOD_TRIANGLES] = GL_TRIANGLES,
	};

	return glDMethods[method];
}

void gldraw_elements(GLDrawMethod method, u64 indiceCount, GLType type, u64 offset) {
	glDrawElements(
		getGlDrawMethod(method), 
		indiceCount, 
		gltypes_getInfo(type).glTarget, 
		(void *)offset);
}

void gldraw_elementsInstanced(GLDrawMethod method, u64 indiceCount, GLType type, u64 offset, u64 instances) {
	glDrawElementsInstanced(
		getGlDrawMethod(method),
		indiceCount,
		gltypes_getInfo(type).glTarget,
		(void *)offset,
		instances);
}
