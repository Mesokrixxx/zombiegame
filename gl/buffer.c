#include "buffer.h"
#include "util/assert.h"
#include <GL/glew.h>

#define INCOMPATIBLE_BUFFER_STR "incompatible buffer"

static u32 getGlType(GLBufferType type) {
	const u32 glTargets[] = {
		[GLBUFFER_TYPE_ARRAY] = GL_ARRAY_BUFFER,
		[GLBUFFER_TYPE_ELEMENTS] = GL_ELEMENT_ARRAY_BUFFER,
		[GLBUFFER_TYPE_VERTEX] = GL_VERTEX_ARRAY,
	};

	return glTargets[type];
}

static u32 getGlUsage(GLBufferUsage usage) {
	const u32 glUsages[] = {
		[GLBUFFER_USAGE_DRAW_STREAM] = GL_STREAM_DRAW,
		[GLBUFFER_USAGE_DRAW_STATIC] = GL_STATIC_DRAW,
		[GLBUFFER_USAGE_DRAW_DYNAMIC] = GL_DYNAMIC_DRAW,
	};

	return glUsages[usage];
}

GLBuffer *glbuffer_create(Allocator *allocator, GLBufferType type) {
	GLBuffer *buffer = allocator_alloc(allocator, sizeof(GLBuffer));
	*buffer = (GLBuffer){
		.allocator = allocator,
		.type = type,
	};

	return buffer;
}

void glbuffer_init(GLBuffer *buffer) {
	u32 handle;

	switch (buffer->type) {
		case (GLBUFFER_TYPE_ARRAY):
		case (GLBUFFER_TYPE_ELEMENTS):
			glGenBuffers(1, &handle);
			break ;

		case (GLBUFFER_TYPE_VERTEX):
			glGenVertexArrays(1, &handle);
			break ;

		default:
			UNREACHABLE();
			return ;
	}
	buffer->handle = handle;
}

void glbuffer_bind(GLBuffer *buffer) {
	static u32 boundBuffers[_GLBUFFER_TYPE_COUNT];

	if (boundBuffers[buffer->type] == buffer->handle)
		return ;

	u32 target = getGlType(buffer->type);
	switch (buffer->type) {
		case (GLBUFFER_TYPE_ARRAY):
		case (GLBUFFER_TYPE_ELEMENTS):
			glBindBuffer(target, buffer->handle);
			break ;

		case (GLBUFFER_TYPE_VERTEX):
			glBindVertexArray(buffer->handle);
			break ;

		default: 
			UNREACHABLE(); 
			return ;
	}
	boundBuffers[buffer->type] = buffer->handle;
}

void glbuffer_unbind(GLBufferType type) {
	GLBuffer buffer = {
		.type = type,
		.handle = 0,
	};
	glbuffer_bind(&buffer);
}

void glbuffer_data(GLBuffer *buffer, u64 dataSize, const void *data, GLBufferUsage usage) {
	glBufferData(getGlType(buffer->type), dataSize, data, getGlUsage(usage));
}

void glbuffer_dataSub(GLBuffer *buffer, u64 offset, u64 dataSize, const void *data) {
	glBufferSubData(getGlType(buffer->type), offset, dataSize, data);
}

void glbuffer_destroy(GLBuffer *buffer) {
	if (!buffer)
		return ;
	switch (buffer->type) {
		case (GLBUFFER_TYPE_ARRAY):
		case (GLBUFFER_TYPE_ELEMENTS):
			glDeleteBuffers(1, &buffer->handle);
			break ;

		case (GLBUFFER_TYPE_VERTEX):
			glDeleteVertexArrays(1, &buffer->handle);
			break ;

		default:
			UNREACHABLE();
			return ;
	}
	allocator_free(buffer->allocator, buffer);
}

void _glbuffer_attribPointerImpl(GLBuffer *, u64 stride, u64 locOffset, GLBufferAttribPointerDesc *descs, u64 descsCount) {
	for (u64 i = 0; i < descsCount; i++) {
		glEnableVertexAttribArray(i + locOffset);
		glVertexAttribPointer(
			i + locOffset, 
			descs[i].count, 
			gltypes_getInfo(descs[i].type).glTarget, 
			GL_FALSE, 
			stride, 
			(void *)descs[i].offset);
		glVertexAttribDivisor(i + locOffset, descs[i].divisor);
	}
}
