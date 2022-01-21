#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include "structures.h"

int main(void) {

	srand(time(NULL));
	initmemory();
	int *out = returnROM("ibm.ch8");
	int size = out[0];
	for (int i = 1; i < size; i++) {
		memory[0x200 + i - 1] = out[i];
	}
	for (int i = 0; i < size; i++) {
		printf("Memory loc %x value is %x\n",0x200+i,memory[0x200+i]);
	}
	free(out);
	while (running) {
		eval(fetch());
	}
	return 0;

}
