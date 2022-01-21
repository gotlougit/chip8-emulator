uint32_t WINFLAGS = SDL_WINDOW_VULKAN;
uint32_t RENDFLAGS = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

const int WIDTH = 1000;
const int HEIGHT = 1000;

void screenInit(void) {

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window *win = SDL_CreateWindow("window",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, WINFLAGS);
	SDL_Renderer *rend = SDL_CreateRenderer(win, -1, RENDFLAGS);
}

void renderWin(int start) {
	
	SDL_RenderClear(rend);
	SDL_SetRenderDrawColor(rend, 255,0,0,255);
	for (int i = 0; i < 100; i++) {
		SDL_RenderDrawPoint(rend, start+i,start+i);
	}
	SDL_RenderPresent(rend);
	
}

screenInit();
