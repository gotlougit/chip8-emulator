#include <time.h>
#include "structures.h"

int main(int argc, char **argv) {

	if (!(argv[1])) {
		fprintf(LOGFILE, "Error! No ROM given to run!\n");
		return 1;
	}

	srand(time(NULL));
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window *win = SDL_CreateWindow(WIN_NAME,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, WINFLAGS);
	SDL_Renderer *rend = SDL_CreateRenderer(win, -1, RENDFLAGS);
	SDL_RenderSetLogicalSize(rend, WIDTH, HEIGHT);
	SDL_Texture *tex = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGB565, SDL_TEXTUREACCESS_STATIC, ORIG_WIDTH, ORIG_HEIGHT);
	memset(pixels, 0, ORIG_PIXEL_COUNT - 1);
	
	initmemory();
	loadROM(argv[1]);
	
	while (running) {
		inputResult inputStuff = getInput();
		
		running = inputStuff.running;
		keyPressed = inputStuff.value;

		time_t beginningTime = time(NULL);
		for (;ipc < INST_PER_SEC; ipc++) {
			if (!(ipc % 60)) {
				updateTimers();
			}
			eval(fetch(), keyPressed, rend, tex);
		}
		if (time(NULL) - beginningTime >= 1) {
			ipc = 0;
		}
	}

	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(rend);
	SDL_DestroyWindow(win);
	SDL_Quit();
	return 0;

}
