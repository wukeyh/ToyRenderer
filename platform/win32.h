#pragma once

#include "../geometry.h"
#include <windows.h>

typedef struct mouse
{
	// for camera orbit
	vec2f orbit_pos;
	vec2f orbit_delta;
	// for first-person view (diabled now)
	vec2f fv_pos;
	vec2f fv_delta;
	// for mouse wheel
	float wheel_delta;
} mouse_t ;

typedef struct window
{
	HWND h_window;
	HDC mem_dc;
	HBITMAP bm_old;
	HBITMAP bm_dib;
	unsigned char* window_fb;
	int width;
	int height;
	char keys[512];
	char buttons[2];	//left button  right button
	int is_close;
	mouse_t mouse_info;

} window_t;

extern window_t* window;

void bm_init(int width, int height);

int window_init(int width, int height, const char* title);

int window_destroy();

int window_draw();

void msg_dispatch();


