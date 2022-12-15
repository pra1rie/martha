#include "nara.h"
#include "image.h"
#include "cursor.h"
#include "config.h"
#include <SDL2/SDL.h>
#include <inttypes.h>
#include <stdlib.h>


Image*
create_image(uint32_t width, uint32_t height, Pixel color)
{
	Image *image = calloc(sizeof(Image), 1);
	image->width = width;
	image->height = height;
	image->pixels = calloc(sizeof(Pixel), width * height);
	fill_image(image, color);
	return image;
}

void
fill_image(Image *image, Pixel color)
{
	for (size_t i = 0; i < (image->width * image->height); ++i) {
		image->pixels[i] = color;
	}
}

void
free_image(Image *image)
{
	free(image->pixels);
	free(image);
}

void
draw_image(Image *image, int x, int y)
{
	SDL_Rect rect;
	SDL_Surface *surface;
	SDL_Texture *texture;

	surface = SDL_CreateRGBSurface(
			0, image->width, image->height, 32,
			0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	SDL_LockSurface(surface);
	memcpy(surface->pixels, image->pixels, surface->h * surface->pitch);
	SDL_UnlockSurface(surface);
	texture = SDL_CreateTextureFromSurface(REN, surface);

	rect = (SDL_Rect) {
		x, y,
		image->width * cursor.zoom,
		image->height * cursor.zoom,
	};
	
	SDL_RenderCopy(REN, texture, NULL, &rect);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
}
