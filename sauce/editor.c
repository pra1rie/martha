#include "editor.h"
#include "image.h"
#include "config.h"
#include "nara.h"
#include <SDL2/SDL.h>


void
editor_init()
{
	SDL_Color color = {0xFA, 0xFA, 0xFA, 0xFF};
	editor.font = fontNew("JuliaMono.ttf", 24, color);
	editor_new_layer(0, create_image(64, 64, 0xFFFFFFFF));
	editor_new_layer(1, create_image(64, 64, 0x00000000));
	editor.current_layer = 1;
	editor.color = 0x000000FF;
}

int
editor_new_layer(size_t layer, Image *image)
{
	if (editor.num_layers >= MAX_LAYERS || layer >= MAX_LAYERS)
		return 1;

	++editor.num_layers;

	for (size_t i = editor.num_layers; i > layer; --i)
		editor.layers[i] = editor.layers[i-1];

	editor.layers[layer] = image;
	editor.current_layer = layer;

	return 0;
}

int
editor_remove_layer(size_t layer)
{
	if (editor.num_layers == 0 || layer >= MAX_LAYERS)
		return 1;
	
	free_image(editor.layers[layer]);
	--editor.num_layers;

	for (size_t i = layer; i < editor.num_layers; ++i)
		editor.layers[i] = editor.layers[i+1];

	editor.current_layer = layer;
	
	return 0;
}
