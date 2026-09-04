#ifndef ENGINE_INPUTS_H
# define ENGINE_INPUTS_H

# include "math/vector2.h"
# include "container/map.h"
# include "engine/time.h"
# include "engine/window.h"

typedef enum : u8 {
	INPUT_INVALID = 0 << 0,
	INPUT_PRESSED = 1 << 0,
	INPUT_RELEASED = 1 << 1,
	INPUT_DOWN = 1 << 2,
	INPUT_EXISTS = 1 << 7,
} InputState;

typedef struct {
	InputState state;
	Time lastUpdate;
} InputInfo;

typedef struct {
	Allocator *allocator;
	Window *window;
	Time now;
	struct {
		V2 pos;
		V2 motion;
		V2 wheel;
	} mouse;
	Dynlist(InputInfo) buttons;
	Dynlist(u32) toClear;
	Map *buttonNames;
	String errorLog;
} Inputs;

Inputs *inputs_create(Allocator *allocator, Window *window);
void inputs_update(Inputs *inputs, Time now);
void inputs_process(Inputs *inputs, const SDL_Event *event);
InputInfo inputs_get(Inputs *inputs, const char *inputStr);
void inputs_destroy(Inputs *inputs);

#endif // ENGINE_INPUTS_H
