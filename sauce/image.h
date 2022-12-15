#pragma once

#include <inttypes.h>

typedef uint32_t Pixel;

typedef struct image {
	uint32_t width, height;
	Pixel *pixels;
} Image;

Image *create_image(uint32_t width, uint32_t height, Pixel color);
void fill_image(Image *image, Pixel color);
void free_image(Image *image);
void draw_image(Image *image, int x, int y);

