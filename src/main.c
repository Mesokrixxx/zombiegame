#include "engine/engine.h"
#include "engine/window.h"
#include "engine/inputs.h"
#include "engine/ecs.h"
#include "util/assert.h"
#include "util/error.h"
#include "util/log.h"
#include "memory/mallocator.h"
#include "src/fonts.h"
#include <math.h>

 typedef struct {
	V2 pos;
	V2 scale;
	f32 z;
	f32 angle;
} TransformCmp;

typedef struct {
	V2 vel;
	f32 accel;
} VelocityCmp;

typedef struct {
	SpriteAtlasID atlasID;
	V2i idx;
	V4 color;
} SpriteCmp;

typedef enum : u8 {
	ENTITY_FLAGS_CONTROLLED = 1 << 0,
	ENTITY_FLAGS_ZOMBIE = 1 << 1,
} EntityFlagsCmp;

# define ENTITY_LAYER -5

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

	ECSEntity player = ecs_newEntity(ecs, (ECSComponentID[]){transformCmp, velocityCmp, spriteCmp, entityFlagsCmp}, 4);
	*(TransformCmp *)ecs_get(ecs, player, transformCmp) = (TransformCmp){
		.pos = v2(500, 500),
		.scale = v2(60, 60),
		.z = ENTITY_LAYER,
	};
	*(VelocityCmp *)ecs_get(ecs, player, velocityCmp) = (VelocityCmp){
		.accel  = 250,
	};
	*(SpriteCmp *)ecs_get(ecs, player, spriteCmp) = (SpriteCmp){
		.atlasID = entitiesAtlas,
		.color = v4(0.66, 0.66, 0.66, 1),
	};
	*(EntityFlagsCmp *)ecs_get(ecs, player, entityFlagsCmp) = ENTITY_FLAGS_CONTROLLED;
	
	V2 *playerPos = &((TransformCmp *)ecs_get(ecs, player, transformCmp))->pos;

	Time last = time_now();

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

		if (inputs_get(inputsHandler, "space").state & INPUT_PRESSED) {
			ECSEntity newZombie = ecs_newEntity(ecs, (ECSComponentID[]){transformCmp, velocityCmp, spriteCmp, entityFlagsCmp}, 4);
			*(TransformCmp *)ecs_get(ecs, newZombie, transformCmp) = (TransformCmp){
				.pos = inputsHandler->mouse.pos,
				.scale = v2(60, 60),
				.z = ENTITY_LAYER,
			};
			*(VelocityCmp *)ecs_get(ecs, newZombie, velocityCmp) = (VelocityCmp){
				.accel  = 200,
			};
			*(SpriteCmp *)ecs_get(ecs, newZombie, spriteCmp) = (SpriteCmp){
				.atlasID = entitiesAtlas,
				.color = v4(0.22, 0.66, 0.22, 1),
			};
			*(EntityFlagsCmp *)ecs_get(ecs, newZombie, entityFlagsCmp) = ENTITY_FLAGS_ZOMBIE;
		}

		ecs_forEach(ecs, ((ECSComponentID[]){transformCmp, velocityCmp, entityFlagsCmp}), 3, it) {
			TransformCmp *transforms = ecs_getComponent(ecs, it.archetype, transformCmp);
			VelocityCmp *velocities = ecs_getComponent(ecs, it.archetype, velocityCmp);
			EntityFlagsCmp *flags = ecs_getComponent(ecs, it.archetype, entityFlagsCmp);
		
			for (u64 i = 0; i < it.entityCount; i++) {
				if (flags[i] & ENTITY_FLAGS_CONTROLLED) {
					if (inputs_get(inputsHandler, "z").state & INPUT_DOWN)
						velocities[i].vel.y += velocities[i].accel * dt;
					if (inputs_get(inputsHandler, "s").state & INPUT_DOWN)
						velocities[i].vel.y -= velocities[i].accel * dt;

					if (inputs_get(inputsHandler, "d").state & INPUT_DOWN)
						velocities[i].vel.x += velocities[i].accel * dt;
					if (inputs_get(inputsHandler, "q").state & INPUT_DOWN)
						velocities[i].vel.x -= velocities[i].accel * dt;

					V2 mousePos = inputsHandler->mouse.pos;
					transforms[i].angle = 
						atan2f(
							mousePos.y - transforms[i].pos.y, 
							mousePos.x - transforms[i].pos.x);
				}

				if (flags[i] & ENTITY_FLAGS_ZOMBIE) {
					f32 angle = 
						atan2f(
							playerPos->y - transforms[i].pos.y,
							playerPos->x - transforms[i].pos.x);
							
					transforms[i].angle = angle;
					velocities[i].vel.x = velocities[i].accel * cosf(angle) * dt;
					velocities[i].vel.y = velocities[i].accel * sinf(angle) * dt;
				}
			}
		}

		ecs_forEach(ecs, ((ECSComponentID[]){transformCmp, velocityCmp}), 2, it) {
			TransformCmp *transforms = ecs_getComponent(ecs, it.archetype, transformCmp);
			VelocityCmp *velocities = ecs_getComponent(ecs, it.archetype, velocityCmp);

			for (u64 i = 0; i < it.entityCount; i++) {
				if (v2_eq(velocities[i].vel, v2(0, 0)))
					continue ;

				transforms[i].pos = v2_add(transforms[i].pos, velocities[i].vel);
				
				f32 slowForce = velocities[i].accel * 1.25 * dt; 
				velocities[i].vel = 
					v2_max(v2(0, 0), v2_sub(velocities[i].vel, v2(slowForce, slowForce)));
			}
		}
		
		ecs_forEach(ecs, ((ECSComponentID[]){transformCmp, spriteCmp}), 2, it) {
			TransformCmp *transforms = ecs_getComponent(ecs, it.archetype, transformCmp);
			SpriteCmp *sprites = ecs_getComponent(ecs, it.archetype, spriteCmp);

			for (u64 i = 0; i < it.entityCount; i++) {
				sprites_add(spritesHandler, sprites[i].atlasID, sprites[i].idx, &(Sprite){
						.pos = transforms[i].pos,
						.scale = transforms[i].scale,
						.z = transforms[i].z,
						.angle = transforms[i].angle,
						.color = sprites[i].color,
					});
			}
		}

		window_clear(window);
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
