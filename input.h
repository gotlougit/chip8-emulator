#include "features.h"

typedef struct {
	int value;
	bool running;
} inputResult;

inputResult getInput(void) {

	int value = -1;
	bool running = true;

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.scancode) {
					case SDL_SCANCODE_ESCAPE:
						running = false;
						break;
					case SDL_SCANCODE_1:
						value = 1;
						break;
					case SDL_SCANCODE_2:
						value = 2;
						break;
					case SDL_SCANCODE_3:
						value = 3;
						break;
					case SDL_SCANCODE_4:
						value = 0xC;
						break;
					case SDL_SCANCODE_Q:
						value = 4;
						break;
					case SDL_SCANCODE_W:
						value = 5;
						break;
					case SDL_SCANCODE_E:
						value = 6;
						break;
					case SDL_SCANCODE_R:
						value = 0xD;
						break;
					case SDL_SCANCODE_A:
						value = 7;
						break;
					case SDL_SCANCODE_S:
						value = 8;
						break;
					case SDL_SCANCODE_D:
						value = 9;
						break;
					case SDL_SCANCODE_F:
						value = 0xE;
						break;
					case SDL_SCANCODE_Z:
						value = 0xA;
						break;
					case SDL_SCANCODE_X:
						value = 0;
						break;
					case SDL_SCANCODE_C:
						value = 0xB;
						break;
					case SDL_SCANCODE_V:
						value = 0xF;
						break;
					default:
						fprintf(LOGFILE, "Unsupported key entered!\n");
						break;
				}
				break;
			default:
				break;
		}

	}
	
	inputResult out;
	out.value = value;
	out.running = running;

	return out;

}
