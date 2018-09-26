#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <SDL.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include "include/chunk.h"
#include "include/superchunk.h"
#include "include/key_mouse_manager.h"
#include "include/physics.h"
#include "include/render.h"
#include "include/textures.h"
#include "include/vectors.h"
#include "include/window_utils.h"
#include "include/shader_utils.h"
#include "include/global_vars.h"
#include "include/resources.h"
#include "include/textures.h"	//Holds the textures in C source
				//Generated in GIMP

glm::vec3 position;
glm::vec3 forward;
glm::vec3 right;
glm::vec3 up;
glm::vec3 lookat;
glm::vec3 angle;

SDL_Window *window;
SDL_GLContext context;

int context_init_finished = 0;

int ww, wh;
int mx, my, mz;
int face;
uint8_t buildtype = 1;

int now;
unsigned int keys = 0;

superchunk *world;

void mainLoop(SDL_Window *window) 
{
	while (true)
	{
		SDL_Event ev;
		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				return;
			case SDL_KEYDOWN:
				key_down(&ev.key);
				break;
			case SDL_KEYUP:
				key_up(&ev.key);
				break;
			case SDL_MOUSEBUTTONDOWN:
				mouse_button_down(&ev.button);
				break;
			case SDL_MOUSEWHEEL:
				mouse_wheel(&ev.wheel);
				break;
			case SDL_MOUSEMOTION:
				mouse_motion(&ev.motion);
				break;
			case SDL_WINDOWEVENT:
				window_event(&ev.window);
				break;
			default:
				break;
			}
		}
		physics();
		render();
		SDL_GL_SwapWindow(window);
	}
}

int main(int argc, char* argv[]) 
{
	SDL_Init(SDL_INIT_VIDEO);

	// Select an OpenGL ES 2.0 profile.
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	window = SDL_CreateWindow("Lol idk",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		640, 480,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	// Set relative mouse mode, this will grab the cursor.
	SDL_SetRelativeMouseMode(SDL_TRUE);

	context = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1);
	reshape(640, 480);

	print_opengl_info();
	printf("Use the mouse to look around.\n");
	printf("Use cursor keys, pageup and pagedown to move around.\n");
	printf("Use home and end to go to two predetermined positions.\n");
	printf("Press the left mouse button to build a block.\n");
	printf("Press the right mouse button to remove a block.\n");
	printf("Use the scrollwheel to select different types of blocks.\n");

	if (!init_resources())
		return EXIT_FAILURE;

	keys |= ~64;
	context_init_finished = 1;

	mainLoop(window);

	free_resources();
	
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
