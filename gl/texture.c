#include "texture.h"
#include "util/assert.h"
#include "util/error.h"
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

static u32 getGlType(GLTextureType type) {
	const u32 types[] = {
		[GLTEXTURE_TYPE_2D] = GL_TEXTURE_2D,
	};

	return types[type];
}

static u32 getGlParameter(GLTextureParameter param) {
	const u32 params[] = {
		[GLTEXTURE_PARAMETER_WRAPS] = GL_TEXTURE_WRAP_S,
		[GLTEXTURE_PARAMETER_WRAPT] = GL_TEXTURE_WRAP_T,
		[GLTEXTURE_PARAMETER_MINFILTER] = GL_TEXTURE_MIN_FILTER,
		[GLTEXTURE_PARAMETER_MAGFILTER] = GL_TEXTURE_MAG_FILTER,
	};

	return params[param];
}

static u32 getGlParameterValue(GLTextureParameterValue value) {
	const u32 values[] = {
		[GLTEXTURE_PARAMETERVALUE_REPEAT] = GL_REPEAT,
		[GLTEXTURE_PARAMETERVALUE_MREPEAT] = GL_MIRRORED_REPEAT,
		[GLTEXTURE_PARAMETERVALUE_CLAMPE] = GL_CLAMP_TO_EDGE,
		[GLTEXTURE_PARAMETERVALUE_CLAMPB] = GL_CLAMP_TO_BORDER,
		[GLTEXTURE_PARAMETERVALUE_NEAREST] = GL_NEAREST,
		[GLTEXTURE_PARAMETERVALUE_LINEAR] = GL_LINEAR,
		[GLTEXTURE_PARAMETERVALUE_NMIPMAPN] = GL_NEAREST_MIPMAP_NEAREST,
		[GLTEXTURE_PARAMETERVALUE_NMIPMAPL] = GL_NEAREST_MIPMAP_LINEAR,
		[GLTEXTURE_PARAMETERVALUE_LMIPMAPN] = GL_LINEAR_MIPMAP_NEAREST,
		[GLTEXTURE_PARAMETERVALUE_LMIPMAPL] = GL_LINEAR_MIPMAP_LINEAR,
	};

	return values[value];
}

GLTexture *gltexture_create(Allocator *allocator, GLTextureType type) {
	GLTexture *tex = allocator_alloc(allocator, sizeof(GLTexture));
	File *texFile = file_create(allocator, NULL);
	*tex = (GLTexture){
		.allocator = allocator,
		.file = texFile,
		.type = type,
	};

	return tex;
}

void gltexture_init(GLTexture *texture) {
	u32 handle;

	switch (texture->type) {
		case (GLTEXTURE_TYPE_2D):
			glGenTextures(1, &handle);
			break ;

		default:
			UNREACHABLE();
			return ;
	}

	texture->handle = handle;
}

void gltexture_bind(GLTexture *texture, u8 loc) {
	static u32 boundTexture[_GLTEXTURE_TYPE_COUNT];

	if (boundTexture[texture->type] == texture->handle)
		return ;

	glActiveTexture(GL_TEXTURE0 + loc);

	u32 target = getGlType(texture->type);
	switch (texture->type) {
		case (GLTEXTURE_TYPE_2D):
			glBindTexture(target, texture->handle);
			break ;

		default:
			UNREACHABLE();
			return ;
	}

	boundTexture[texture->type] = texture->handle;
}

void gltexture_unbind(GLTexture *texture, u8 loc) {
	GLTexture tex = { 
		.type = texture->type 
	};
	gltexture_bind(&tex, loc);
}

void gltexture_set(GLTexture *tex, GLTextureParameter param, GLTextureParameterValue value) {
	u32 target = getGlType(tex->type);

	switch (param) {
		case (GLTEXTURE_PARAMETER_WRAPS):
		case (GLTEXTURE_PARAMETER_WRAPT):
		case (GLTEXTURE_PARAMETER_MINFILTER):
		case (GLTEXTURE_PARAMETER_MAGFILTER):
			glTexParameteri(target, getGlParameter(param), getGlParameterValue(value));
			break ;
		
		default:
			UNREACHABLE();
			break ;
	}
}

bool gltexture_generate(GLTexture *texture, const char *path, bool generateMipmap) {
	file_setPath(texture->file, path);
	if (!file_load(texture->file))
		return false;

	u32 target = getGlType(texture->type);
	int width, height, channels;

	stbi_set_flip_vertically_on_load(true);
	u8 *data = 
		stbi_load_from_memory(
			(u8 *)texture->file->content, string_length(texture->file->content), &width, &height, &channels, 0);
	if (!data) {
		error_msgSet("convert image to usable data");
		return false;
	}
	texture->size = v2i(width, height);

	glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	if (generateMipmap)
		glGenerateMipmap(target);

	stbi_image_free(data);

	return true;
}

void gltexture_destroy(GLTexture *tex) {
	if (!tex)
		return ;
	switch (tex->type) {
		case GLTEXTURE_TYPE_2D:
			glDeleteTextures(1, &tex->handle);
			break ;

		default:
			UNREACHABLE();
			return ;
	}
	file_destroy(tex->file);
	allocator_free(tex->allocator, tex);
}
