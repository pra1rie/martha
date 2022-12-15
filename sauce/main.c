#include "nara.h"
#include "image.h"
#include "cursor.h"
#include "editor.h"
#include "config.h"

#include <stdlib.h>
#include <inttypes.h>


static struct {
	bool alt, shift, ctrl, super;
	bool left_click, right_click, middle_click;
} keys;

static void handle_draw_pixel(int x, int y);
static void events(SDL_Event ev);
static void update(void);
static void init(void);
static void quit(void);

// TODO: add tools like lines, circles, rectangles, bucket...
//			also draw with different sizes (cursor.w, cursor.h)

static void
handle_draw_pixel(int x, int y)
{
	Image *layer = editor.layers[editor.current_layer];
	int index = y * layer->width + x;

	if (x < 0 || x >= layer->width || y < 0 || y >= layer->width)
		return;

	// pencil
	if (keys.left_click) {
		layer->pixels[index] = editor.color;
	}
	// eraser
	else if (keys.right_click) {
		layer->pixels[index] = 0x00000000;
	}
	// colour picker
	else if (keys.middle_click && layer->pixels[index] != 0x00000000) {
		editor.color = layer->pixels[index];
	}
}

#define GET_SPECIAL_KEYS(b) { \
		SDL_Keycode scumcode = ev.key.keysym.sym; \
		if (scumcode == SDLK_LALT || scumcode == SDLK_RALT) \
			keys.alt = b; \
		if (scumcode == SDLK_LCTRL || scumcode == SDLK_RCTRL) \
			keys.ctrl = b; \
		if (scumcode == SDLK_LSHIFT || scumcode == SDLK_RSHIFT) \
			keys.shift = b; \
		if (scumcode == SDLK_LGUI || scumcode == SDLK_RGUI) \
			keys.super = b; \
		}

#define GET_MOUSE_BUTTONS(b) { \
		Uint8 button = ev.button.button; \
		if (button == SDL_BUTTON_LEFT) \
			keys.left_click = b; \
		if (button == SDL_BUTTON_RIGHT) \
			keys.right_click = b; \
		if (button == SDL_BUTTON_MIDDLE) \
			keys.middle_click = b; \
		}

static void
events(SDL_Event ev)
{
	switch (ev.type) {
	case SDL_QUIT:
		global.isRunning = false;
		break;
	case SDL_KEYDOWN:
		GET_SPECIAL_KEYS(true);
		break;
	case SDL_KEYUP:
		GET_SPECIAL_KEYS(false);
		break;
	case SDL_MOUSEBUTTONDOWN:
		GET_MOUSE_BUTTONS(true);
		break;
	case SDL_MOUSEBUTTONUP:
		GET_MOUSE_BUTTONS(false);
		break;
	case SDL_MOUSEWHEEL:
		if (keys.ctrl) {
			cursor.zoom += ev.wheel.y;
			if (cursor.zoom <= 1) cursor.zoom = 1;
		}
		else if (keys.shift) {
			cursor.offset_x += ev.wheel.y * cursor.scroll_speed;
		}
		else {
			cursor.offset_y += ev.wheel.y * cursor.scroll_speed;
		}
		break;
	case SDL_WINDOWEVENT:
		if (ev.window.event == SDL_WINDOWEVENT_RESIZED) {
			WIN.width  = ev.window.data1;
			WIN.height = ev.window.data2;
		}
		break;
	default: break;
	}
}

static void
update(void)
{
	char text[100];
	int pos_x, pos_y;
	// update
	
	/* cursor.scroll_speed = cursor.zoom * SCROLL_SPEED; */
	/* cursor.scroll_speed = WIN.width / editor.layers[editor.current_layer]->width * 2; */

	SDL_GetMouseState(&cursor.x, &cursor.y);

	pos_x = (cursor.x - cursor.offset_x) / cursor.zoom;
	pos_y = (cursor.y - cursor.offset_y) / cursor.zoom;

	sprintf(text, "pos: %d %d", pos_x, pos_y);
	handle_draw_pixel(pos_x, pos_y);

	// render
	SDL_SetRenderDrawColor(REN, UNWRAP(0x242430FF));
	SDL_RenderClear(REN);

	for (size_t i = 0; i < editor.num_layers; ++i)
		draw_image(editor.layers[i], cursor.offset_x, cursor.offset_y);

	SDL_SetRenderDrawColor(REN, UNWRAP(0x242430FF));
	fontWriteRight(&editor.font, text, WIN.width, WIN.height - editor.font.size);
	
	SDL_RenderPresent(REN);
}

static void
quit(void)
{
	for (size_t i = 0; i < editor.num_layers; ++i)
		free_image(editor.layers[i]);

	fontFree(&editor.font);
	windowFree(&WIN);
	exitNara();
}

static void
init(void)
{
	initNara();
	WIN = windowNew("Martha", 800, 600, true);
	editor_init();
	cursor_init();
}

int
main(void)
{
	init();
	
	while (global.isRunning) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev))
			events(ev);
		update();
	}

	quit();
}
