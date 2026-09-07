#include "engine/engine.h"
#include "engine/window.h"
#include "engine/inputs.h"
#include "util/assert.h"
#include "util/error.h"
#include "util/log.h"
#include "memory/mallocator.h"
#include "src/sprites.h"
#include "src/fonts.h"
#include "src/ecs.h"

typedef struct {
	V2 pos;
	V2 scale;
	f32 z;
} TransformCmp;
typedef struct {
	V2 vel;
	f32 speed;
} VelocityCmp;
typedef struct {
	SpriteAtlasID atlasID;
	V2i idx;
	V4 color;
} SpriteCmp;
typedef enum : u8 {
	ENTITY_FLAGS_NONE = 0 << 0,
	ENTITY_FLAGS_CONTROLLED = 1 << 0,
} EntityFlagsCmp;

int main() {
	Mallocator mallocator;
	mallocator_init(&mallocator);

	Engine *engine = engine_create(&mallocator);
	ASSERT(engine_init(engine), "failed to init engine: %s", error_msgGet());

	Window *window = window_create(&mallocator, v2i(1080, 720));
	ASSERT(window_init(window, "Zombie Game"), "failed to create window: %s", error_msgGet());

	window_backgroundColor(window, v4(1, 1, 1, 1));
	window_vSync(window, true);

	Inputs *inputsHandler = inputs_create(&mallocator, window);

	Sprites *spritesHandler = sprites_create(&mallocator, "res/shaders/sprite.glsl");
	ASSERT(sprites_init(spritesHandler), "failed to init sprite handler: %s", error_msgGet());

	Fonts *fontsHandler = fonts_create(&mallocator, spritesHandler, v2i(16, 16));
	ASSERT(fonts_init(fontsHandler, "res/textures/fonts.png"), "failed to init font handler: %s", error_msgGet());

	ECS *ecs = ecs_create(&mallocator);
	ECSComponentID transformCmp = ecs_registerComponent(ecs, sizeof(TransformCmp));
	ECSComponentID velocityCmp = ecs_registerComponent(ecs, sizeof(VelocityCmp));
	ECSComponentID spriteCmp = ecs_registerComponent(ecs, sizeof(SpriteCmp));
	ECSComponentID entityFlagsCmp = ecs_registerComponent(ecs, sizeof(EntityFlagsCmp));

	SpriteAtlasID entitiesAtlas = sprites_registerAtlas(spritesHandler, v2i(32, 32), "res/textures/entities.png");
	ASSERT(entitiesAtlas, "failed to create atlas: %s", error_msgGet());

	Mat4 projectionMat, viewMat;
	mat4_ortho(&projectionMat, v2(0, window->size.x), v2(0, window->size.y), v2(0.1, 100));
	mat4_identity(&viewMat);

	glshader_bind(spritesHandler->shader);
	glshader_setUniformMat4(spritesHandler->shader, "projection", &projectionMat);
	glshader_setUniformMat4(spritesHandler->shader, "view", &viewMat);

	Time last = time_now();

	ECSEntity player = ecs_newEntity(ecs, (ECSComponentID[]){ transformCmp, velocityCmp, spriteCmp, entityFlagsCmp }, 4);
	*(TransformCmp *)ecs_get(ecs, player, transformCmp) = (TransformCmp){
		.pos = v2(500, 500),
		.scale = v2(60, 60),
		.z = -5,
	};
	*(VelocityCmp *)ecs_get(ecs, player, velocityCmp) = (VelocityCmp){
		.speed  = 250,
	};
	*(SpriteCmp *)ecs_get(ecs, player, spriteCmp) = (SpriteCmp){
		.atlasID = entitiesAtlas,
		.color = v4(0.66, 0.66, 0.66, 1),
	};
	*(EntityFlagsCmp *)ecs_get(ecs, player, entityFlagsCmp) = ENTITY_FLAGS_CONTROLLED;
	

	engine->running = true;
	while (engine->running) {		
		Time now = time_now();
		Time dt = now - last;
		last = now;

		window_update(window);
		inputs_update(inputsHandler, now);
		sprites_update(spritesHandler);
		
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			engine_process(engine, &ev);
			window_process(window, &ev);
			inputs_process(inputsHandler, &ev);
		}

		if (window->resized) {
			mat4_ortho(&projectionMat, v2(0, window->size.x), v2(0, window->size.y), v2(0.1, 100));
			glshader_bind(spritesHandler->shader);
			glshader_setUniformMat4(spritesHandler->shader, "projection", &projectionMat);
		}

		window_clear(window);

		ecs_forEach(ecs, ((ECSComponentID[]){ velocityCmp, entityFlagsCmp }), it) {
			VelocityCmp *velocities = ecs_getComponent(ecs, it.archetype, velocityCmp);
			EntityFlagsCmp *flags = ecs_getComponent(ecs, it.archetype, entityFlagsCmp);
		
			for (u64 i = 0; i < it.entityCount; i++) {
				if (flags[i] & ENTITY_FLAGS_CONTROLLED) {
					if (inputs_get(inputsHandler, "z").state & INPUT_DOWN)
						velocities[i].vel.y += velocities[i].speed * dt;
					if (inputs_get(inputsHandler, "s").state & INPUT_DOWN)
						velocities[i].vel.y -= velocities[i].speed * dt;

					if (inputs_get(inputsHandler, "d").state & INPUT_DOWN)
						velocities[i].vel.x += velocities[i].speed * dt;
					if (inputs_get(inputsHandler, "q").state & INPUT_DOWN)
						velocities[i].vel.x -= velocities[i].speed * dt;
				}
			}
		}

		ecs_forEach(ecs, ((ECSComponentID[]){ transformCmp, velocityCmp }), it) {
			TransformCmp *transforms = ecs_getComponent(ecs, it.archetype, transformCmp);
			VelocityCmp *velocities = ecs_getComponent(ecs, it.archetype, velocityCmp);

			for (u64 i = 0; i < it.entityCount; i++) {
				transforms->pos = v2_add(transforms[i].pos, velocities[i].vel);
				
				f32 slowForce = velocities[i].speed * 1.25 * dt; 
				velocities[i].vel = 
					v2_max(v2(0, 0), v2_sub(velocities[i].vel, v2(slowForce, slowForce)));
			}
		}
		
		ecs_forEach(ecs, ((ECSComponentID[]){ transformCmp, spriteCmp }), it) {
			TransformCmp *transforms = ecs_getComponent(ecs, it.archetype, transformCmp);
			SpriteCmp *sprites = ecs_getComponent(ecs, it.archetype, spriteCmp);

			for (u64 i = 0; i < it.entityCount; i++) {
				sprites_add(spritesHandler, sprites[i].atlasID, sprites[i].idx, &(Sprite){
						.pos = transforms[i].pos,
						.scale = transforms[i].scale,
						.z = transforms[i].z,
						.color = sprites[i].color,
					}, 1);
			}
		}
		
		sprites_draw(spritesHandler);
		window_swap(window);
	}

	LOG("allocated %zu bytes (%zu Ko) from %zu allocations",
		mallocator.used, mallocator.used >> 10, mallocator.allocCount);

	ecs_destroy(ecs);
	fonts_destroy(fontsHandler);
	sprites_destroy(spritesHandler);
	inputs_destroy(inputsHandler);
	window_destroy(window);
	engine_destroy(engine);
	
	if (mallocator.used) {
		WARN("leak of %zu bytes: %zu / %zu frees", 
			mallocator.used, mallocator.freeCount, mallocator.allocCount);
	}
	
	return 0;
}
