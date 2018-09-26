#ifndef KEY_MOUSE_MANAGER_H
#define KEY_MOUSE_MANAGER_H

extern unsigned int keys;

void key_down(SDL_KeyboardEvent *ev);
void key_up(SDL_KeyboardEvent *ev);
void mouse_motion(SDL_MouseMotionEvent *ev);
void mouse_wheel(SDL_MouseWheelEvent *ev);
void mouse_button_down(SDL_MouseButtonEvent *ev);

#endif
