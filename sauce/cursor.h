#pragma once


struct {
	int x, y, w, h;
	int offset_x, offset_y;
	int zoom, scroll_speed;
} cursor;

void cursor_init();

