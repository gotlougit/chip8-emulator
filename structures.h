#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "display.h"

//implemented as macros to clean up code

#define getOPCODE(inst) (inst / 0x1000)
#define getX(inst) ((inst / 0x100) % 0x10)
#define getY(inst) ((inst / 0x10) % 0x10)
#define getN(inst) (inst % 0x10)
#define getNN(inst,N) ((0x10*getY(inst)) + N)
#define getNNN(inst,NN) ((0x100*getX(inst)) + NN)

int ipc = 0;
int reset = 0;

uint8_t registers[REG_SIZE];

uint8_t delayTimer = 100;
uint8_t soundTimer = 100;

int stacklast = -1;
uint16_t stack[STACK_SIZE];

uint16_t indexreg = INIT_MEM;
uint16_t pc = INIT_MEM;
uint8_t memory[MEM_SIZE];

bool running = true;

uint8_t font[80] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

uint8_t reverseNum(uint8_t num) {

	uint8_t out = 0;
	int iter = 7;
	while (num > 0) {
		out += (uint8_t) pow(2,iter) * (num % 2);
		num = num / 2;
		iter--;
	}
	return out;

}

void loadROM(char *loc) {
	
	FILE *fp;

	fp = fopen(loc,"rb");
	
	if (fp != NULL) {
		int i = 0, c;
		while ((c = fgetc(fp)) != EOF) {
			memory[INIT_MEM + i] = c;
			fprintf(stderr, "Loaded memory address %x, value is %x\n",INIT_MEM + i, c);
			i++;
		}
	}
	fclose(fp);

}

void initmemory(void) {
	
	for (int i = 0x0; i < INIT_MEM; i++) {
		memory[i] = 0;
		if (0x50 <= i && i < 0xA0) {
			memory[i] = font[i-0x50];
		}
	}

}

void updateTimers(void) {

	if (delayTimer > 0) {
		delayTimer--;
	}
	if (soundTimer > 0) {
		//make beep sound
		soundTimer--;
	}

	fprintf(stderr, "Delay timer: %d, Sound timer: %d\n",delayTimer,soundTimer);

}

/*
int push(int value) {
	
	if (stacklast + 1 < STACK_SIZE ) {
		stack[stacklast+1] = value;
		stacklast += 1;
		return 0;
	} else {
		return 1;
	}

}

int pop(void) {
	
	if (stacklast >= 0) {
		stacklast -= 1;
	} else {
		return 1;
	}
	return stack[stacklast+1];

}
*/

int fetch(void) {

	fprintf(stderr,"memory: 0x%x\n",pc);
	if (pc <= MEM_SIZE) {
		return (0x100 * memory[pc]) + memory[pc+1];
	} else {
		fprintf(stderr, "Tried to access out of bounds memory, setting back to %x\n", INIT_MEM);
		reset++;
		if (RESET_THRESHOLD && reset > RESET_THRESHOLD) {
			fprintf(stderr, "Threshold reached, exiting process\n");
			running = false;
		}
		pc = INIT_MEM;
		fetch();
	}
	
}

void eval(int inst, SDL_Renderer *rend, SDL_Texture *tex) {

	fprintf(stderr,"inst: %x ",inst);
	switch (inst) {
		case 0xE0:
			memset(pixels, 0, ORIG_PIXEL_COUNT - 1);
			SDL_RenderClear(rend);
			SDL_RenderPresent(rend);
			break;
		/*
		case 0xEE:
			pc = pop();
			break;
		*/
		default:
			int OPCODE = getOPCODE(inst);
			int X = getX(inst);
			int Y = getY(inst);
			int N = getN(inst);
			int NN = getNN(inst,N);
			int NNN = getNNN(inst,NN);
			switch (OPCODE) {
				/*
				case 2: //so it falls through to case 1
					push(pc);
				*/
				case 1:
					pc = NNN - 2;
					break;
				/*
				case 3:
					if (registers[mask(inst, 2)] == (inst & 0x00FF)) {
						pc+=2;
					}
					break;
				case 4:
					if (registers[mask(inst, 2)] != (inst & 0x00FF)) {
						pc++;
					}
					break;
				case 5:
					if (registers[mask(inst,2)] == registers[mask(inst,3)]) {
						pc++;
					}
					break;
				*/
				case 6:
					registers[X] = NN;
					break;
				case 7:
					registers[X] += NN;
					break;
				/*
				case 8:
					switch (inst & 0x000F) {
						case 0: //set equal to
							registers[mask(inst,2)] = registers[mask(inst,3)];
							break;
						case 1: //bitwise OR
							registers[mask(inst,2)] = registers[mask(inst,2)] | registers[mask(inst,3)];
							break;
						case 2: //bitwise AND
							registers[mask(inst,2)] = registers[mask(inst,2)] & registers[mask(inst,3)];
							break;
						case 3: //logical OR
							registers[mask(inst,2)] = registers[mask(inst,2)] || registers[mask(inst,3)];
							break;
						case 4: //add
							int sum = registers[mask(inst,2)] + registers[mask(inst,3)];
							registers[0xF] = (sum > 0xFF)?1:0;
							registers[mask(inst,2)] = sum;
							break;
						case 6: //shift right
							if (EIGHTXY_VY) {
								registers[mask(inst,2)] = registers[mask(inst,3)];
							}
							registers[0xF] = mask(registers[mask(inst,2)],4);
							registers[mask(inst,2)] = registers[mask(inst,2)] >> 1;
							break;
						case 14: //shift left
							if (EIGHTXY_VY) {
								registers[mask(inst,2)] = registers[mask(inst,3)];
							}
							registers[0xF] = mask(registers[mask(inst,2)],4);
							registers[mask(inst,2)] = registers[mask(inst,2)] << 1;

							break;

					}
					break;
				case 9:
					if (registers[mask(inst,2)] != registers[mask(inst,3)]) {
						pc++;
					}
					break;
				*/
				case 0xA:
					indexreg = NNN;
					fprintf(stderr, "indexreg set to: %x\n", indexreg);
					break;
				/*
				case 0xB:
					if (BXNN) {
						pc = (inst & 0x0FFF) + registers[mask(inst,2)];
					} else {
						pc = (inst & 0x0FFF) + registers[0];
					}
					break;
				case 0xC:
					registers[mask(inst,2)] = (inst & 0xFF) && (rand());		
					break;
				*/
				case 0xD: //screen drawing
					bool hasScreenChanged = false;
					registers[0xF] = 0;
					int y = registers[Y] % ORIG_HEIGHT;
					for (int iter = 0; iter < N; iter++) {
						int x = registers[X] % ORIG_WIDTH;
						uint8_t row = (memory[indexreg + iter]);
						while (row > 0) {
							bool bit = (bool) (row % 2);
							bool pixelVal = getPixelVal(x,y);
							bool newVal = bit ^ pixelVal;
							if (newVal != pixelVal) {
								hasScreenChanged = true;
							}
							registers[0xF] = (!(newVal) && pixelVal);
							setPixel(x,y,newVal);
							x++;
							if (x > ORIG_WIDTH) {
								x = 0;
								break;
							}
							row = row / 2;
						}
						y++;
						if (y > ORIG_HEIGHT) {
							y = 0;
							break;
						}
					}
					if (hasScreenChanged) { //only redraw if screen changes
						SDL_RenderClear(rend);
						SDL_UpdateTexture(tex, NULL, pixels, ORIG_WIDTH * sizeof(uint16_t));
						SDL_RenderCopy(rend, tex, NULL, NULL);
						SDL_RenderPresent(rend);
					}
					break;
				default: //means this is unknown
					if (inst) {
						fprintf(stderr,"Unknown instruction, maybe data: %x\n",inst);
					} else {
						fprintf(stderr,"Memory is zero/0\n");
					}
					break;
			}
			break;

	}
	pc+=2;
}
