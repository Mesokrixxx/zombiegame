#ifndef WINDOW_H
# define WINDOW_H

# include "math/vector2.h"
# include "math/vector4.h"
# include "container/string.h"
# include <SDL3/SDL_events.h>

typedef struct {
	Allocator *allocator;
	V2i size;
	void *handle;
	void *glctx;
	u32 toClear;
	bool resized;
	String errorLog;
} Window;

Window *window_create(Allocator *alloc, V2i size);
bool window_init(Window *window, const char *title);
void window_backgroundColor(Window *window, V4 color);
void window_update(Window *window);
void window_process(Window *window, const SDL_Event *ev);
void window_clear(Window *window);
void window_swap(Window *window);
void window_vSync(Window *window, bool enable);
void window_destroy(Window *window);

#endif // WINDOW_H
