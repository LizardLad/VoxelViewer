#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#include <SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

extern glm::vec3 position;
extern glm::vec3 forward;
extern glm::vec3 right;
extern glm::vec3 up;
extern glm::vec3 lookat;
extern glm::vec3 angle;

extern SDL_Window *window;
extern SDL_GLContext context;

extern int context_init_finished;

extern int ww, wh;
extern int mx, my, mz;
extern int face;
extern uint8_t buildtype;

extern int now;

#endif
