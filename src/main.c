#include "engine/engine.h"
#include "engine/window.h"
#include "engine/inputs.h"
#include "util/assert.h"
#include "util/error.h"
#include "util/log.h"
#include "memory/mallocator.h"
#include "src/sprites.h"
#include "src/fonts.h"

int main() {
	Mallocator mallocator;
	mallocator_init(&mallocator);

	Engine *engine = engine_create(&mallocator);
	ASSERT(engine_init(engine), "failed to init engine: %s", error_msgGet());

	Window *window = window_create(&mallocator, v2i(1080, 720));
	ASSERT(window_init(window, "Zombie Game"), "failed to create window: %s", error_msgGet());

	Inputs *inputs = inputs_create(&mallocator, window);

	Sprites *sprites = sprites_create(&mallocator, "res/shaders/sprite.glsl");
	ASSERT(sprites_init(sprites), "failed to init sprite handler: %s", error_msgGet());
	SpriteAtlasID entitiesAtlas = sprites_registerAtlas(sprites, v2i(32, 32), "res/textures/entities.png");
	ASSERT(entitiesAtlas, "failed to create atlas: %s", error_msgGet());

	Fonts *fonts = fonts_create(&mallocator, sprites, v2i(16, 16));
	ASSERT(fonts_init(fonts, "res/textures/fonts.png"), "failed to init font handler: %s", error_msgGet());

	window_setBackgroundColor(window, v4(1, 1, 1, 1));

	Mat4 projectionMat, viewMat;
	mat4_ortho(&projectionMat, v2(0, window->size.x), v2(0, window->size.y), v2(0.1, 100));
	mat4_identity(&viewMat);
	mat4_translate(&viewMat, v3(0, 0, -1));

	glshader_bind(sprites->shader);
	glshader_setUniformMat4(sprites->shader, "projection", &projectionMat);
	glshader_setUniformMat4(sprites->shader, "view", &viewMat);

	engine->running = true;
	while (engine->running) {
		Time now = time_now();
		
		window_update(window);
		inputs_update(inputs, now);
		sprites_update(sprites);
		
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			engine_process(engine, &ev);
			window_process(window, &ev);
			inputs_process(inputs, &ev);
		}

		if (window->resized) {
			mat4_ortho(&projectionMat, v2(0, window->size.x), v2(0, window->size.y), v2(0.1, 100));
			glshader_bind(sprites->shader);
			glshader_setUniformMat4(sprites->shader, "projection", &projectionMat);
		}

		window_clear(window);

		sprites_add(sprites, entitiesAtlas, v2i(0, 0), &(Sprite){ v2(500, 500), v2(60, 60), 0, v4(1, 1, 1, 1) }, 1);

		sprites_draw(sprites);
		window_swap(window);
	}

	LOG("allocated %zu bytes (%zu Ko)", mallocator.used, mallocator.used >> 10);

	fonts_destroy(fonts);
	sprites_destroy(sprites);
	inputs_destroy(inputs);
	window_destroy(window);
	engine_destroy(engine);
	
	if (mallocator.used)
		WARN("leak of %zu bytes", mallocator.used);
	
	return 0;
}
