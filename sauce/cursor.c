#include "cursor.h"
#include "editor.h"
#include "config.h"
#include "nara.h"


void
cursor_init()
{
	cursor.x = cursor.y = 0;
	cursor.scroll_speed = SCROLL_SPEED;
	cursor.zoom = 8;
	cursor.offset_x = (global.width - (editor.layers[0]->width * cursor.zoom)) / 2;
	cursor.offset_y = (global.height - (editor.layers[0]->height * cursor.zoom)) / 2;
}
