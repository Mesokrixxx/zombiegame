#ifndef SPRITES_H
# define SPRITES_H

# include "memory/allocator.h"
# include "gl/shader.h"
# include "gl/buffer.h"
# include "gl/texture.h"
#include "container/string.h"

typedef u64 SpriteTexture;

typedef struct {
	V2 pos;
	V2 scale;
	f32 z;
} Sprite;

typedef struct {
	Allocator *allocator;
	Dynlist(GLTexture*) textures;
	GLShader *shader;
	GLBuffer *vao;
	GLBuffer *vbo;
	GLBuffer *ibo;
	GLBuffer *ebo;
	Dynlist(Sprite) sprites;
	String errorLog;
} Sprites;

Sprites *sprites_create(Allocator *allocator, const char *shaderPath);
bool sprites_init(Sprites *sprites);
SpriteTexture sprites_registerTexture(Sprites *sprites, const char *path);
void sprites_update(Sprites *sprites);
bool sprites_add(Sprites *sprites, Sprite *data);
void sprites_draw(Sprites *sprites);
void sprites_destroy(Sprites *sprites);

#endif // SPRITES_H
