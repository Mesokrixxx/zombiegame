#ifndef GL_BUFFER_H
# define GL_BUFFER_H

# include "memory/allocator.h"
# include "gl/types.h"

typedef enum : u8 {
	GLBUFFER_TYPE_ARRAY,
	GLBUFFER_TYPE_ELEMENTS,
	GLBUFFER_TYPE_VERTEX,
	_GLBUFFER_TYPE_COUNT,
} GLBufferType;

typedef enum : u8 {
	GLBUFFER_USAGE_DRAW_STREAM,
	GLBUFFER_USAGE_DRAW_STATIC,
	GLBUFFER_USAGE_DRAW_DYNAMIC,
} GLBufferUsage;

typedef struct {
	u64 count;
	GLType type;
	u64 offset;
	u64 divisor;
} GLBufferAttribPointerDesc;

typedef struct {
	Allocator *allocator;
	GLBufferType type;
	u32 handle;
} GLBuffer;

GLBuffer *glbuffer_create(Allocator *allocator, GLBufferType type);
void glbuffer_init(GLBuffer *buffer);
void glbuffer_bind(GLBuffer *buffer);
void glbuffer_unbind(GLBuffer *buffer);
void glbuffer_data(GLBuffer *buffer, u64 dataSize, const void *data, GLBufferUsage usage);
void glbuffer_dataSub(GLBuffer *buffer, u64 offset, u64 dataSize, const void *data);
void glbuffer_destroy(GLBuffer *buffer);

# define glbuffer_attribPointer(buf, stride, locOffset, descs) \
	_glbuffer_attribPointerImpl((buf), (stride), (locOffset), (descs), ARRAY_SIZE(descs))

void _glbuffer_attribPointerImpl(GLBuffer *buffer, u64 stride, u64 locOffset, GLBufferAttribPointerDesc *descs, u64 descsCount);

#endif // GL_BUFFER_H
