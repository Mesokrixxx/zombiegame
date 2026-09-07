#include "sprites.h"
#include "math/vector2.h"
#include "util/error.h"
#include "gl/draw.h"

typedef struct {
	V2 pos;
	V2 uv;
} Vertex;

typedef struct {
	Sprite data;

} SpriteInner;

# define MAX_SPRITES 256

Sprites *sprites_create(Allocator *allocator, const char *path) {
	Sprites *sprites = allocator_alloc(allocator, sizeof(Sprites));
	String errorLog = string_create(allocator, NULL, STRING_ERRORLOG_MINSIZE);
	GLShader *shader = glshader_create(allocator, path);
	GLBuffer *vao = glbuffer_create(allocator, GLBUFFER_TYPE_VERTEX);
	GLBuffer *vbo = glbuffer_create(allocator, GLBUFFER_TYPE_ARRAY);
	GLBuffer *ibo = glbuffer_create(allocator, GLBUFFER_TYPE_ARRAY);
	GLBuffer *ebo = glbuffer_create(allocator, GLBUFFER_TYPE_ELEMENTS);
	*sprites = (Sprites){
		.allocator = allocator,
		.shader = shader,
		.vao = vao,
		.vbo = vbo,
		.ibo = ibo,
		.ebo = ebo,
		.errorLog = errorLog,
	};

	dynlist_init(allocator, sprites->atlases, 8);
	return sprites;
}

bool sprites_init(Sprites *sprites) {
	if (!glshader_compile(sprites->shader))
		return false;
	
	glbuffer_init(sprites->vao);
	glbuffer_bind(sprites->vao);

	Vertex vertices[] = {
		{ v2(-0.5, -0.5), v2(0, 0) },
		{ v2(-0.5,  0.5), v2(0, 1) },
		{ v2( 0.5, -0.5), v2(1, 0) },
		{ v2( 0.5,  0.5), v2(1, 1) },
	};
	u8 indices[] = { 0, 1, 2, 1, 2, 3 };
	
	glbuffer_init(sprites->vbo);
	glbuffer_bind(sprites->vbo);
	glbuffer_data(sprites->vbo, sizeof(vertices), vertices, GLBUFFER_USAGE_DRAW_STATIC);
	glbuffer_attribPointer(
		sprites->vbo, sizeof(Vertex), 0, 
		((GLBufferAttribPointerDesc[]){
			{ .count = 2, .type = GLTYPE_F32 },
			{ .count = 2, .type = GLTYPE_F32, .offset = offsetof(Vertex, uv) }
		}));

	glbuffer_init(sprites->ebo);
	glbuffer_bind(sprites->ebo);
	glbuffer_data(sprites->ebo, sizeof(indices), indices, GLBUFFER_USAGE_DRAW_STATIC);

	glbuffer_init(sprites->ibo);
	glbuffer_bind(sprites->ibo);
	glbuffer_data(sprites->ibo, sizeof(Sprite) * MAX_SPRITES, NULL, GLBUFFER_USAGE_DRAW_DYNAMIC);
	glbuffer_attribPointer(
		sprites->ibo, sizeof(Sprite), 2,
		((GLBufferAttribPointerDesc[]){
			{ .count = 2, .type = GLTYPE_F32, .divisor = 1 },
			{ .count = 2, .type = GLTYPE_F32, .offset = offsetof(Sprite, scale), .divisor = 1 },
			{ .count = 1, .type = GLTYPE_F32, .offset = offsetof(Sprite, z), .divisor = 1 },
			{ .count = 4, .type = GLTYPE_F32, .offset = offsetof(Sprite, color), .divisor = 1},
			{ .count = 2, .type = GLTYPE_F32, .offset = offsetof(Sprite, _uvMin), .divisor = 1},
			{ .count = 2, .type = GLTYPE_F32, .offset = offsetof(Sprite, _uvMax), .divisor = 1},
		}));
	return true;
}

void sprites_update(Sprites *sprites) {
	dynlist_forEach(sprites->atlases, it) {
		dynlist_clear(it.elem->sprites);
	}
}

SpriteAtlasID sprites_registerAtlas(Sprites *sprites, V2i spriteSize, const char *path) {
	u64 listSize = dynlist_size(sprites->atlases);

	GLTexture *texture = gltexture_create(sprites->allocator, GLTEXTURE_TYPE_2D);
	gltexture_init(texture);
	gltexture_bind(texture, 0);
	if (!gltexture_generate(texture, path, false)) {
		gltexture_unbind(texture, listSize);
		gltexture_destroy(texture);
		return 0;
	}
	gltexture_set(texture, GLTEXTURE_PARAMETER_WRAPS, GLTEXTURE_PARAMETERVALUE_REPEAT);
	gltexture_set(texture, GLTEXTURE_PARAMETER_WRAPT, GLTEXTURE_PARAMETERVALUE_REPEAT);
	gltexture_set(texture, GLTEXTURE_PARAMETER_MINFILTER, GLTEXTURE_PARAMETERVALUE_NEAREST);
	gltexture_set(texture, GLTEXTURE_PARAMETER_MAGFILTER, GLTEXTURE_PARAMETERVALUE_NEAREST);
	
	V2 normalizedStep = v2(1 / (f32)texture->size.x, 1 / (f32)texture->size.y);
	SpriteAtlas atlas = {
		.texture = texture,
		.spriteCount = v2i_div(texture->size, spriteSize),
		.normalizedSpriteStep = v2(normalizedStep.x * spriteSize.x, normalizedStep.y * spriteSize.y),
	};
	dynlist_init(sprites->allocator, atlas.sprites, MAX_SPRITES);

	dynlist_pushBack(sprites->atlases, &atlas);
	return listSize + 1;
}

bool sprites_add(Sprites *sprites, SpriteAtlasID atlasID, V2i atlasIndex, Sprite *data, u64 spriteCount) {
	SpriteAtlas *atlas = &sprites->atlases[atlasID - 1];

	if (dynlist_size(atlas->sprites) >= MAX_SPRITES) {
		string_assign(sprites->errorLog, "max sprites count reached: %zu", MAX_SPRITES);
		error_msgSet(sprites->errorLog);
		return false;
	}

	atlasIndex.y = atlas->spriteCount.y - atlasIndex.y - 1;
	data->_uvMin = v2(atlas->normalizedSpriteStep.x * atlasIndex.x, atlas->normalizedSpriteStep.y * atlasIndex.y);
	data->_uvMax = v2_add(data->_uvMin, atlas->normalizedSpriteStep);
	dynlist_pushBackArray(atlas->sprites, data, spriteCount);
	return true;
}

void sprites_draw(Sprites *sprites) {
	glshader_bind(sprites->shader);
	glbuffer_bind(sprites->vao);
	glbuffer_bind(sprites->ibo);
	dynlist_forEach(sprites->atlases, it) {
		u64 lSize = dynlist_size(it.elem->sprites);
		if (!lSize)
			continue ;

		gltexture_bind(it.elem->texture, 0);
		glbuffer_dataSub(sprites->ibo, 0, lSize * sizeof(Sprite), it.elem->sprites);
		gldraw_elementsInstanced(GLDRAW_METHOD_TRIANGLES, 6, GLTYPE_U8, 0, lSize);
	}
}

void sprites_destroy(Sprites *sprites) {
	if (!sprites)
		return ;
	dynlist_forEach(sprites->atlases, it) {
		gltexture_destroy(it.elem->texture);
		dynlist_destroy(it.elem->sprites);
	}
	dynlist_destroy(sprites->atlases);
	glbuffer_destroy(sprites->vao);
	glbuffer_destroy(sprites->vbo);
	glbuffer_destroy(sprites->ibo);
	glbuffer_destroy(sprites->ebo);
	glshader_destroy(sprites->shader);
	string_destroy(sprites->errorLog);
	allocator_free(sprites->allocator, sprites);
}
