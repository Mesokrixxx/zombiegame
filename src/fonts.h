#ifndef FONTS_H
# define FONTS_H

# include "src/sprites.h"

typedef Sprite Font;

typedef struct {
	Allocator *allocator;
	Sprites *sprites;
	SpriteAtlasID atlasID;
	V2i fontSize;
} Fonts;

Fonts *fonts_create(Allocator *allocator, Sprites *sprites, V2i fontSize);
bool fonts_init(Fonts *fonts, const char *fontAtlasPath);
void fonts_add(Fonts *fonts, Font *data, char c);
void fonts_addStr(Fonts *fonts, Font *data, const char *str);
void fonts_destroy(Fonts *fonts);

#endif // FONTS_H
