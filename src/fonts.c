#include "fonts.h"

Fonts *fonts_create(Allocator *allocator, Sprites *sprites, V2i fontSize) {
	Fonts *fonts = allocator_alloc(allocator, sizeof(Fonts));
	*fonts = (Fonts){
		.allocator = allocator,
		.sprites = sprites,
		.fontSize = fontSize
	};

	return fonts;
}

bool fonts_init(Fonts *fonts, const char *fontAtlasPath) {
	SpriteAtlasID id = sprites_registerAtlas(fonts->sprites, fonts->fontSize, fontAtlasPath);
	if (!id)
		return false;

	fonts->atlasID = id;
	return true;
}

static V2i findIndex(char c) {
	const char *chars[] = {
		"ABCDEFGHIJKLMNOP",
		"QRSTUVWXYZ*-+/!?",
		"0123456789#@$%,.",
		"abcdefghijklmnop",
		"qrstuvwxyz:;<>()",
		"[]{}\"\'~\\^_|"
	};

	for (u64 i = 0; i < ARRAY_SIZE(chars); i++) {
		for (u64 j = 0; chars[i][j]; j++) {
			if (chars[i][j] != c)
				continue ;
			return v2i(j, i);
		}
	}
	return v2i(-1, -1);
} 

void fonts_add(Fonts *fonts, Font *data, char c) {
	V2i index = findIndex(c);
	if (index.x < 0)
		index = findIndex('?');

	sprites_add(fonts->sprites, fonts->atlasID, index, data, 1);
}

void fonts_addStr(Fonts *fonts, Font *data, const char *str) {
	V2 initialPos = data->pos;

	for (u64 i = 0; str[i]; i++) {
		char c = str[i];

		if (c == '\n') {
			data->pos = v2(initialPos.x, data->pos.y + fonts->fontSize.y);
			continue ;
		}
		if (c != ' ')
			fonts_add(fonts, data, c);
		data->pos.x += fonts->fontSize.x;	
	}
}

void fonts_destroy(Fonts *fonts) {
	if (!fonts)
		return ;
	allocator_free(fonts->allocator, fonts);
}
