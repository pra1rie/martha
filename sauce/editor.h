#pragma once

#include "nara_font.h"
#include "image.h"
#include "config.h"
#include <inttypes.h>


struct {
	Image *layers[MAX_LAYERS];
	size_t num_layers;
	size_t current_layer;
	Pixel color;
	Font font;
} editor;

void editor_init();
int editor_new_layer(size_t layer, Image *image);
int editor_remove_layer(size_t layer);

