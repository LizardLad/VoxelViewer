#include <stdbool.h>

#include <SDL_opengles2.h>

#include "include/global_vars.h"
#include "include/key_mouse_manager.h"

void reshape(int w, int h) {
	ww = w;
	wh = h;
	glViewport(0, 0, w, h);
}

bool mouse_in_window = true;

void window_event(SDL_WindowEvent *ev) {
	switch(ev->event) {
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			reshape(ev->data1, ev->data2);
			break;
		case SDL_WINDOWEVENT_ENTER:
			mouse_in_window = true;
			if(keys & ~64)
			{
				SDL_SetRelativeMouseMode(SDL_TRUE);
			}
			break;
		case SDL_WINDOWEVENT_LEAVE:
			mouse_in_window = false;
			break;
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			if(mouse_in_window)
			{
				SDL_SetRelativeMouseMode(SDL_TRUE);
				keys &= ~64;
			}
			break;
		default:
			break;
	}
}
