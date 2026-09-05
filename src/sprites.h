#ifndef SPRITES_H
# define SPRITES_H

# include "memory/allocator.h"
# include "gl/shader.h"
# include "gl/buffer.h"
# include "gl/texture.h"
# include "container/string.h"
# include "math/vector4.h"

typedef struct {
	V2 pos;
	V2 scale;
	f32 z;
	V4 color;
	V2 _uvMin, _uvMax;
} Sprite;

typedef struct {
	GLTexture *texture;
	Dynlist(Sprite) sprites;
	V2i spriteCount;
	V2 normalizedSpriteStep;
} SpriteAtlas;

typedef u64 SpriteAtlasID;

typedef struct {
	Allocator *allocator;
	Dynlist(SpriteAtlas) atlases;
	GLShader *shader;
	GLBuffer *vao;
	GLBuffer *vbo;
	GLBuffer *ibo;
	GLBuffer *ebo;
	String errorLog;
} Sprites;

Sprites *sprites_create(Allocator *allocator, const char *shaderPath);
bool sprites_init(Sprites *sprites);
SpriteAtlasID sprites_registerAtlas(Sprites *sprites, V2i spriteSize, const char *path);
void sprites_update(Sprites *sprites);
bool sprites_add(Sprites *sprites, SpriteAtlasID atlasID, V2i atlasIndex, Sprite *data, u64 spriteCount);
void sprites_draw(Sprites *sprites);
void sprites_destroy(Sprites *sprites);

#endif // SPRITES_H
