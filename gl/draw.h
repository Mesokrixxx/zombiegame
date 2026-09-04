#ifndef GL_DRAW_H
# define GL_DRAW_H

# include "gl/types.h"

typedef enum {
	GLDRAW_METHOD_TRIANGLES,
} GLDrawMethod;

void gldraw_elements(GLDrawMethod method, u64 indiceCount, GLType type, u64 offset);
void gldraw_elementsInstanced(GLDrawMethod method, u64 indiceCount, GLType type, u64 offset, u64 instances);

#endif // GL_DRAW_H
