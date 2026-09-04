#include "window.h"
#include "util/error.h"
#include <SDL3/SDL_video.h>
#include <GL/glew.h>

Window *window_create(Allocator *alloc, V2i size) {
	Window *window = allocator_alloc(alloc, sizeof(Window));
	String errorLogStr = string_create(alloc, NULL, STRING_ERRORLOG_MINSIZE);
	*window = (Window){
		.allocator = alloc,
		.size = size,
		.toClear = GL_COLOR_BUFFER_BIT,
		.errorLog = errorLogStr,
	};

	return window;
}

bool window_init(Window *window, const char *title) {
	SDL_Window *handle = 
		SDL_CreateWindow(
			title, 
			window->size.x, window->size.y, 
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!handle) {
		string_assign(window->errorLog, "sdl handle: %s", SDL_GetError());
		error_msgSet(window->errorLog);
		return false;
	}

	SDL_GLContext glctx = SDL_GL_CreateContext(handle);
	if (!glctx) {
		string_assign(window->errorLog, "gl context: %s", SDL_GetError());
		error_msgSet(window->errorLog);
		SDL_DestroyWindow(handle);
		return false;
	}

	glewExperimental = true;

	u32 glewErr = glewInit();
	if (glewErr != GLEW_OK && glewErr != GLEW_ERROR_NO_GLX_DISPLAY) {
		string_assign(window->errorLog, 
			"glew init (code: %u): %s", glewErr, glewGetErrorString(glewErr));
		error_msgSet(window->errorLog);
		SDL_GL_DestroyContext(glctx);
		SDL_DestroyWindow(handle);
		return false;
	}

	window->handle = handle;
	window->glctx = glctx;
	
	return true;
} 

void window_setBackgroundColor(Window *, V4 color) {
	glClearColor(color.r, color.g, color.b, color.a);
}

void window_update(Window *window) {
	window->resized = false;
}

void window_process(Window *window, const SDL_Event *event) {
	switch (event->type) {
		case (SDL_EVENT_WINDOW_RESIZED):
			window->size = v2i(event->window.data1, event->window.data2);
			window->resized = true;
			glViewport(0, 0, window->size.x, window->size.y);
			break ;
	}
}

void window_clear(Window *window) {
	glClear(window->toClear);
}

void window_swap(Window *window) {
	SDL_GL_SwapWindow(window->handle);
}

void window_destroy(Window *window) {
	if (!window)
		return ;
	SDL_GL_DestroyContext(window->glctx);
	SDL_DestroyWindow(window->handle);
	string_destroy(window->errorLog);
	allocator_free(window->allocator, window);
}
