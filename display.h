#include "input.h"

//macros to make life easier

#define getPixelAddr(x, y) (x + ORIG_WIDTH*y)
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
