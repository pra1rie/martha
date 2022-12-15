#pragma once

// BRUH ZONE

#define SCROLL_SPEED 24
#define MAX_LAYERS 10

#define WIN global.window
#define REN global.window.render

#define UNWRAP(color) (color>>(8*3))&0xFF, \
							 (color>>(8*2))&0xFF, \
				      	 (color>>(8*1))&0xFF, \
				      	 (color>>(8*0))&0xFF
