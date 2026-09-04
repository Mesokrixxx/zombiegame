#include "time.h"
#include <SDL3/SDL_timer.h>

Time time_now() {
	static Time first;
	static Time freq;
	Time now;

	if (!freq)
		freq = SDL_GetPerformanceFrequency();
	now = SDL_GetPerformanceCounter();
	if (!first) {
		first = now;
		return 0;
	}

	return (now - first) / (f64)freq;
}
