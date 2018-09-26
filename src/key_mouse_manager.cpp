#include <stdio.h>
#include <SDL.h>
#include <glm/glm.hpp>

#include "include/superchunk.h"
#include "include/key_mouse_manager.h"
#include "include/global_vars.h"
#include "include/vectors.h"
#include "include/resources.h"

void key_down(SDL_KeyboardEvent *ev)
{
	switch(ev->keysym.scancode)
	{
		case SDL_SCANCODE_LEFT:
		case SDL_SCANCODE_A:
			keys |= 1;
			break;
		case SDL_SCANCODE_RIGHT:
		case SDL_SCANCODE_D:
			keys |= 2;
			break;
		case SDL_SCANCODE_UP:
		case SDL_SCANCODE_W:
			keys |= 4;
			break;
		case SDL_SCANCODE_DOWN:
		case SDL_SCANCODE_S:
			keys |= 8;
			break;
		case SDL_SCANCODE_PAGEUP:
		case SDL_SCANCODE_SPACE:
			keys |= 16;
			break;
		case SDL_SCANCODE_PAGEDOWN:
		case SDL_SCANCODE_C:
			keys |= 32;
			break;
		case SDL_SCANCODE_HOME:
			position = glm::vec3(0, CY + 1, 0);
			angle = glm::vec3(0, -0.5, 0);
			update_vectors();
			break;
		case SDL_SCANCODE_END:
			position = glm::vec3(0, CX * SCX, 0);
			angle = glm::vec3(0, -M_PI * 0.49, 0);
			update_vectors();
			break;
		default:
			break;
	}
}

void key_up(SDL_KeyboardEvent *ev)
{
	switch(ev->keysym.scancode)
	{
		case SDL_SCANCODE_LEFT:
		case SDL_SCANCODE_A:
			keys &= ~1;
			break;
		case SDL_SCANCODE_RIGHT:
		case SDL_SCANCODE_D:
			keys &= ~2;
			break;
		case SDL_SCANCODE_UP:
		case SDL_SCANCODE_W:
			keys &= ~4;
			break;
		case SDL_SCANCODE_DOWN:
		case SDL_SCANCODE_S:
			keys &= ~8;
			break;
		case SDL_SCANCODE_PAGEUP:
		case SDL_SCANCODE_SPACE:
			keys &= ~16;
			break;
		case SDL_SCANCODE_PAGEDOWN:
		case SDL_SCANCODE_C:
			keys &= ~32;
			break;
		case SDL_SCANCODE_DELETE:
			if(context_init_finished)
			{
				free_resources();
				SDL_GL_DeleteContext(context);
				SDL_DestroyWindow(window);
				SDL_Quit();
				exit(EXIT_SUCCESS);
			}
			break;
		case SDL_SCANCODE_ESCAPE:
			SDL_SetRelativeMouseMode(SDL_FALSE);
			keys |= 64;
			break;
		default:
			break;
	}
}

void mouse_motion(SDL_MouseMotionEvent *ev)
{
	if((keys & ~64) && SDL_GetRelativeMouseMode() == SDL_TRUE)
	{
		static const float mousespeed = 0.001;

		angle.x -= ev->xrel * mousespeed;
		angle.y -= ev->yrel * mousespeed;
		
		if(angle.x < -M_PI)
			angle.x += M_PI * 2;
		if(angle.x > M_PI)
			angle.x -= M_PI * 2;
		if(angle.y < -M_PI / 2)
			angle.y = -M_PI / 2;
		if(angle.y > M_PI / 2)
			angle.y = M_PI / 2;

		update_vectors();
	}
}

void mouse_wheel(SDL_MouseWheelEvent *ev)
{
	if((keys & ~64) && SDL_GetRelativeMouseMode() == SDL_TRUE)
	{
		if(ev->y)
		{
			if(ev->y > 0)
				buildtype--;
			else
				buildtype++;
	
			buildtype &= 0xf;
			fprintf(stderr, "Building blocks of type %u (%s)\n", buildtype, blocknames[buildtype]);
			return;
		}
	}
}

void mouse_button_down(SDL_MouseButtonEvent *ev)
{
	if((keys & ~64) && SDL_GetRelativeMouseMode() == SDL_TRUE)
	{
		fprintf(stderr, "Clicked on %d, %d, %d, face %d, button %d\n", mx, my, mz, face, ev->button);

		if(ev->button == SDL_BUTTON_LEFT)
		{
			if(face == 0)
				mx++;
			if(face == 3)
				mx--;
			if(face == 1)
				my++;
			if(face == 4)
				my--;
			if(face == 2)
				mz++;
			if(face == 5)
				mz--;
			world->set(mx, my, mz, buildtype);
		}
		else if(ev->button == SDL_BUTTON_RIGHT)
		{
			world->set(mx, my, mz, 0);
		}
	}
}
