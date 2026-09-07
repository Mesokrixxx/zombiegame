#include "inputs.h"
#include "util/error.h"

#define KEYNAME_MAXLEN 32
#define INVALID_INPUT ((InputInfo){ 0 })

Inputs *inputs_create(Allocator *allocator, Window *window) {
	Inputs *inputs = allocator_alloc(allocator, sizeof(Inputs));
	String errorLog = string_create(allocator, NULL, STRING_ERRORLOG_MINSIZE);
	Map *map = map_create(allocator, sizeof(char[KEYNAME_MAXLEN]), sizeof(u64), map_hash_fixxedStr, map_cmp_fixxedStr);
	*inputs = (Inputs){
		.allocator = allocator,
		.window = window,
		.buttonNames = map,
		.errorLog = errorLog
	};

	dynlist_init(allocator, inputs->buttons, SDL_SCANCODE_COUNT + 64);
	memset(inputs->buttons, 0, dynlist_capacity(inputs->buttons) * sizeof(*inputs->buttons));
	dynlist_init(allocator, inputs->toClear, 8);
	return inputs;
}

void inputs_update(Inputs *inputs, Time now) {
	inputs->now = now;
	inputs->mouse.motion = v2(0, 0);
	inputs->mouse.wheel = v2(0, 0);
	
	dynlist_forEach(inputs->toClear, it) {
		inputs->buttons[*it.elem].state &= ~(INPUT_PRESSED | INPUT_RELEASED);
	}
	dynlist_clear(inputs->toClear);
}

void inputs_process(Inputs *inputs, const SDL_Event *event) {
	switch (event->type) {
		case (SDL_EVENT_MOUSE_WHEEL):
			inputs->mouse.wheel = 
				v2_add(
					inputs->mouse.wheel, 
					v2(event->wheel.x, event->wheel.y));
			break ;

		case (SDL_EVENT_MOUSE_MOTION):
			inputs->mouse.motion = 
				v2_add(
					inputs->mouse.motion,
					v2(event->motion.xrel, -event->motion.yrel));
			inputs->mouse.pos = v2(event->motion.x, inputs->window->size.y - event->motion.y - 1);
			break ;

		case (SDL_EVENT_KEY_DOWN):
		case (SDL_EVENT_KEY_UP):
		case (SDL_EVENT_MOUSE_BUTTON_DOWN):
		case (SDL_EVENT_MOUSE_BUTTON_UP): {
			bool down;
			u64 i;

			if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
				down = event->type == SDL_EVENT_KEY_DOWN;
				i = event->key.scancode;
			}
			else {
				down = event->type == SDL_EVENT_MOUSE_BUTTON_DOWN;
				i = SDL_SCANCODE_COUNT + event->button.button - 1;
			}

			u8 nState = INPUT_EXISTS;
			if (!(inputs->buttons[i].state & INPUT_DOWN) && down)
				nState |= (INPUT_PRESSED | INPUT_DOWN);
			else if (inputs->buttons[i].state & INPUT_DOWN && !down)
				nState |= INPUT_RELEASED;
			else
				break ;

			inputs->buttons[i] = (InputInfo) {
				.state = nState,
				.lastUpdate = inputs->now,
			};
			dynlist_pushBack(inputs->toClear, &i);
		} break ;
	}
}

InputInfo inputs_get(Inputs *inputs, const char *inputStr) {
	char buf[KEYNAME_MAXLEN];

	strncpy(buf, inputStr, KEYNAME_MAXLEN);
	if (buf[KEYNAME_MAXLEN - 1]) {
		string_assign(inputs->errorLog, "str too long: max: %zu chars", KEYNAME_MAXLEN - 1);
		error_msgSet(inputs->errorLog);
		return INVALID_INPUT;
	}
	
	u64 *mapVal = map_get(inputs->buttonNames, buf);
	if (!mapVal) {
		SDL_Keycode key = SDL_GetKeyFromName(buf);
		if (key == SDL_SCANCODE_UNKNOWN) {
			error_msgSet("unknown key");
			return INVALID_INPUT;
		}
		
		u64 code = SDL_GetScancodeFromKey(key, NULL);
		
		inputs->buttons[code].state |= INPUT_EXISTS;
		if (!map_insert(inputs->buttonNames, buf, &code))
			return INVALID_INPUT;
		
		return inputs->buttons[code];	
	}
	
	return inputs->buttons[*mapVal];
}

void inputs_destroy(Inputs *inputs) {
	if (!inputs)
		return ;
	dynlist_destroy(inputs->buttons);
	dynlist_destroy(inputs->toClear);
	map_destroy(inputs->buttonNames);
	string_destroy(inputs->errorLog);
	allocator_free(inputs->allocator, inputs);
}
