#pragma once

#include <inttypes.h>


struct {
	uint32_t x, y, w, h;
	int offset_x, offset_y;
	int zoom, scroll_speed;
} cursor;

void cursor_init();

