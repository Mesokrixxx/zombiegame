#ifndef GL_TEXTURE_H
# define GL_TEXTURE_H

# include "util/file.h"

typedef enum : u8 {
	GLTEXTURE_TYPE_2D,
	_GLTEXTURE_TYPE_COUNT
} GLTextureType;

typedef enum : u8 {
	GLTEXTURE_PARAMETER_WRAPS,
	GLTEXTURE_PARAMETER_WRAPT,
	GLTEXTURE_PARAMETER_MINFILTER,
	GLTEXTURE_PARAMETER_MAGFILTER,
} GLTextureParameter;

typedef enum : u8 {
	GLTEXTURE_PARAMETERVALUE_REPEAT,
	GLTEXTURE_PARAMETERVALUE_MREPEAT,
	GLTEXTURE_PARAMETERVALUE_CLAMPE,
	GLTEXTURE_PARAMETERVALUE_CLAMPB,
	GLTEXTURE_PARAMETERVALUE_NEAREST,
	GLTEXTURE_PARAMETERVALUE_LINEAR,
	GLTEXTURE_PARAMETERVALUE_NMIPMAPN,
	GLTEXTURE_PARAMETERVALUE_NMIPMAPL,
	GLTEXTURE_PARAMETERVALUE_LMIPMAPN,
	GLTEXTURE_PARAMETERVALUE_LMIPMAPL,
} GLTextureParameterValue;

typedef struct {
	Allocator *allocator;
	File *file;
	GLTextureType type;
	u32 handle;
} GLTexture;

GLTexture *gltexture_create(Allocator *allocator, GLTextureType type);
void gltexture_init(GLTexture *texture);
void gltexture_bind(GLTexture *texture, u8 loc);
void gltexture_set(GLTexture *texture, GLTextureParameter param, GLTextureParameterValue value);
bool gltexture_generate(GLTexture *texture, const char *path, bool generateMipmap);
void gltexture_destroy(GLTexture *tex);

#endif // GL_TEXTURE_H
