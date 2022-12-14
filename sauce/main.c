#include "nara.h"
#include <stdlib.h>
#include <inttypes.h>

#define wn global.window
#define rn global.window.render

#define UNWRAP(color) (color>>(8*3))&0xFF, \
							 (color>>(8*2))&0xFF, \
				      	 (color>>(8*1))&0xFF, \
				      	 (color>>(8*0))&0xFF

#define MAX_LAYERS 10
#define SCROLL_SPEED 24

typedef Uint32 Pixel;

static struct {
	bool alt, shift, ctrl, super;
	bool left_click, right_click, middle_click;
} keys;

static struct {
	Uint32 x, y, w, h;
	int offset_x, offset_y;
	int zoom, scroll_speed;
} cursor;

typedef struct image {
	Uint32 width, height;
	Pixel *pixels;
} Image;

static struct {
	Image *layers[MAX_LAYERS];
	size_t num_layers;
	size_t current_layer;
	Pixel color;
	Font font;
} editor;

static Image *create_image(Uint32 width, Uint32 height, Pixel color);
static void fill_image(Image *image, Pixel color);
static void free_image(Image *image);
static void draw_image(Image *image, int x, int y);

static int editor_new_layer(size_t layer, Image *image);
static int editor_remove_layer(size_t layer);

static void events(SDL_Event ev);
static void update(void);
static void init(void);
static void quit(void);


static int
editor_new_layer(size_t layer, Image *image)
{
	if (editor.num_layers >= MAX_LAYERS || layer >= MAX_LAYERS)
		return 1;

	++editor.num_layers;

	for (size_t i = editor.num_layers; i > layer; --i) {
		editor.layers[i] = editor.layers[i-1];
	}

	editor.layers[layer] = image;
	editor.current_layer = layer;

	return 0;
}

static int
editor_remove_layer(size_t layer)
{
	if (editor.num_layers == 0 || layer >= MAX_LAYERS)
		return 1;
	
	free_image(editor.layers[layer]);
	--editor.num_layers;

	for (size_t i = layer; i < editor.num_layers; ++i) {
		editor.layers[i] = editor.layers[i+1];
	}

	editor.current_layer = layer;
	
	return 0;
}


static Image*
create_image(Uint32 width, Uint32 height, Pixel color)
{
	Image *image = calloc(sizeof(Image), 1);
	image->width = width;
	image->height = height;
	image->pixels = calloc(sizeof(Pixel), width * height);
	fill_image(image, color);
	return image;
}

static void
fill_image(Image *image, Pixel color)
{
	for (size_t i = 0; i < (image->width * image->height); ++i) {
		image->pixels[i] = color;
	}
}

static void
free_image(Image *image)
{
	free(image->pixels);
	free(image);
}

static void
draw_image(Image *image, int x, int y)
{
	SDL_Rect rect;
	SDL_Surface *surface;
	SDL_Texture *texture;
	size_t image_size;

	/* image_size = image->width * image->height; */
	surface = SDL_CreateRGBSurface(
			0, image->width, image->height, 32,
			0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	SDL_LockSurface(surface);

	memcpy(surface->pixels, image->pixels, surface->h * surface->pitch);
	/* for (size_t i = 0; i < image_size; ++i) { */
	/* 	rect = (SDL_Rect) { */
	/* 		.x = i / image_size, */
	/* 		.y = i % image_size, */
	/* 		.w = 1, */
	/* 		.h = 1 */
	/* 	}; */

	/* 	SDL_FillRect(surface, &rect, */
	/* 			SDL_MapRGBA(surface->format, UNWRAP(image->pixels[i]))); */
	/* } */
	
	SDL_UnlockSurface(surface);
	texture = SDL_CreateTextureFromSurface(rn, surface);

	rect = (SDL_Rect) {
		x, y,
		image->width * cursor.zoom,
		image->height * cursor.zoom,
	};
	SDL_RenderCopy(rn, texture, NULL, &rect);

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
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
			wn.width  = ev.window.data1;
			wn.height = ev.window.data2;
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
	/* cursor.scroll_speed = wn.width / editor.layers[editor.current_layer]->width * 2; */

	SDL_GetMouseState(&cursor.x, &cursor.y);

	pos_x = (cursor.x - cursor.offset_x) / cursor.zoom;
	pos_y = (cursor.y - cursor.offset_y) / cursor.zoom;

	sprintf(text, "pos: %d %d", pos_x, pos_y);

	if (pos_x >= 0 && pos_y >= 0) {
		Image *layer = editor.layers[editor.current_layer];
		int index = pos_y * layer->width + pos_x;

		if (keys.left_click) {
			layer->pixels[index] = editor.color;
		}
		else if (keys.right_click) {
			layer->pixels[index] = 0x00000000;
		}
	}

	// render
	SDL_SetRenderDrawColor(rn, UNWRAP(0x242430FF));
	SDL_RenderClear(rn);

	for (size_t i = 0; i < editor.num_layers; ++i)
		draw_image(editor.layers[i], cursor.offset_x, cursor.offset_y);

	SDL_SetRenderDrawColor(rn, UNWRAP(0x242430FF));
	fontWriteRight(&editor.font, text, wn.width, wn.height - editor.font.size);
	
	SDL_RenderPresent(rn);
}

static void
quit(void)
{
	for (size_t i = 0; i < editor.num_layers; ++i)
		free_image(editor.layers[i]);

	fontFree(&editor.font);
	windowFree(&wn);
	exitNara();
}

static void
init(void)
{
	initNara();
	wn = windowNew("Martha", 800, 600, true);
	SDL_Color color = {0xFA, 0xFA, 0xFA, 0xFF};
	editor.font = fontNew("JuliaMono.ttf", 24, color);
	
	editor_new_layer(0, create_image(64, 64, 0xFFFFFFFF));
	editor_new_layer(1, create_image(64, 64, 0x00000000));
	editor.current_layer = 1;
	editor.color = 0x000000FF;
	
	cursor.x = cursor.y = 0;
	cursor.scroll_speed = SCROLL_SPEED;
	cursor.zoom = 8;
	cursor.offset_x = (wn.width - (editor.layers[0]->width * cursor.zoom)) / 2;
	cursor.offset_y = (wn.height - (editor.layers[0]->height * cursor.zoom)) / 2;
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
