#include "engine.h"
#include "util/error.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_hints.h>

Engine *engine_create(Allocator *allocator) {
	Engine *engine = allocator_alloc(allocator, sizeof(Engine));
	String errorLogStr = string_create(allocator, NULL, STRING_ERRORLOG_MINSIZE);
	*engine = (Engine){
		.allocator = allocator,
		.errorLog = errorLogStr,
	};

	return engine;
}

bool engine_init(Engine *engine) {
	if (engine->initialized) {
		string_assign(engine->errorLog, "already initialized");
		error_msgSet(engine->errorLog);
		return false;
	}

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		string_assign(engine->errorLog, "sdl initialization: %s", SDL_GetError());
		error_msgSet(engine->errorLog);
		return false;
	}

	engine->initialized = true;
	return true;
}

void engine_process(Engine *engine, const SDL_Event *ev) {
	switch (ev->type) {
		case (SDL_EVENT_QUIT):
			engine->running = false;
			break ;
	}
}

void engine_destroy(Engine *engine) {
	if (!engine)
		return ;
	SDL_Quit();
	string_destroy(engine->errorLog);
	allocator_free(engine->allocator, engine);
}
