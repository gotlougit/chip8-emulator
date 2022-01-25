#include <SDL2/SDL.h>
#include "features.h"

#define getPixelAddr(x, y) (y + ORIG_WIDTH*x)
#define getPixelRow(x,y) (pixels[getPixelAddr(x,y)])
#define getPixelVal(x,y) ((getPixelRow(x,y) ? true : false))

uint16_t pixels[ORIG_PIXEL_COUNT];

void setPixel(int x, int y, bool pixel) {

	if (pixel) {
		pixels[getPixelAddr(x,y)] = (uint16_t) pow(2,16) - 1;		
	} else {
		pixels[getPixelAddr(x,y)] = 0;
	}

}
