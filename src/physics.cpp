#include "include/global_vars.h"
#include "include/key_mouse_manager.h"

void physics() {
	static int prev = 0;
	static const float movespeed = 10;

	now = SDL_GetTicks();
	float dt = (now - prev) * 1.0e-3;
	prev = now;
	
	if(keys & 1)
		position -= right * movespeed * dt;
	if(keys & 2)
		position += right * movespeed * dt;
	if(keys & 4)
		position += forward * movespeed * dt;
	if(keys & 8)
		position -= forward * movespeed * dt;
	if(keys & 16)
		position.y += movespeed * dt;
	if(keys & 32)
		position.y -= movespeed * dt;
}
