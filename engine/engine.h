#ifndef ENGINE_H
# define ENGINE_H

# include "container/string.h"
# include <SDL3/SDL_events.h>

typedef struct {
	Allocator *allocator;
	bool initialized;
	bool running;
	String errorLog;
} Engine;

Engine *engine_create(Allocator *allocator);
bool engine_init(Engine *engine);
void engine_process(Engine *engine, const SDL_Event *ev);
void engine_destroy(Engine *engine);

#endif // ENGINE_H
