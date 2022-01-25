#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "display.h"

int ipc = 0;
int reset = 0;

uint8_t registers[REG_SIZE];

uint8_t delayTimer = 100;
uint8_t soundTimer = 100;

int stacklast = -1;
uint16_t stack[STACK_SIZE];

uint16_t indexreg = INIT_MEM;
uint16_t memory[MEM_SIZE];
uint16_t pc = INIT_MEM;

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

int mask(uint16_t num, int digit) {

	/*
	printf("num: %x",num);
	int x = (num & (0xF * (int) pow(16,4 - digit))) / ((int) pow(16,4 - digit));
	int y = ((num << (4-digit)) % 0x10);
	printf("x: %x, y: %x\n");
	if (x == y) {
		printf("MASK EQUAL!!!");
	}
	return x;
	*/
	return (num & (0xF * (int) pow(16,4 - digit))) / ((int) pow(16,4 - digit));
}

int * returnROM(char *loc) {
	
	FILE *fp;

	fp = fopen(loc,"rb");
	
	if (fp != NULL) {
		int c;
		fseek(fp, 0L, SEEK_END);
		int size = ftell(fp);
		rewind(fp);
		int *out = malloc(size*sizeof(int)+1);
		out[0] = size/2;
		int i = 1;
		while ((c = fgetc(fp)) != EOF) {
			out[i] = c * pow(16,2);
			c = fgetc(fp);
			out[i] += c;
			i++;
		}
		fclose(fp);
		return out;
	} else {
		fclose(fp);
		return NULL;
	}


}

void loadROM(int *rom) {
	
	for (int i = 1; i < rom[0]; i++) {
		memory[INIT_MEM + i - 1] = rom[i];
		fprintf(stderr,"Loaded, memory loc %x value is %04x\n",INIT_MEM+i-1,memory[INIT_MEM+i-1]);
	}
	free(rom);

}

void loadfont(void) {
	
	for (int i = 0x50; i < 0xA0; i++) {
		memory[i] = font[i-0x50];
	}

}

void initmemory(void) {
	
	for (int i = 0x0; i < 0x50; i++) {
		memory[i] = 0;
	}
	loadfont();
	for (int i = 0xA0; i < INIT_MEM; i++) {
		memory[i] = 0;
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

int fetch(void) {

	fprintf(stderr,"memory: 0x%x\n",pc);
	pc+=1;
	if (pc <= MEM_SIZE) {
		return memory[pc-1];
	} else {
		fprintf(stderr, "Tried to access out of bounds memory, setting back to %x\n", INIT_MEM);
		reset++;
		if (RESET_THRESHOLD && reset > RESET_THRESHOLD) {
			fprintf(stderr, "Threshold reached, exiting process\n");
			running = false;
		}
		pc = INIT_MEM;
		return memory[INIT_MEM];
	}
	
}

void eval(int inst, SDL_Renderer *rend, SDL_Texture *tex) {

	fprintf(stderr,"inst: %x ",inst);
	switch (inst) {
		case 0xE0:
			SDL_RenderClear(rend);
			SDL_RenderPresent(rend);
			break;
		case 0xEE:
			pc = pop();
			break;
		default:
			int firstnibble = mask(inst, 1);
			switch (firstnibble) {
				case 2: //so it falls through to case 1
					push(pc);
				case 1:
					pc = inst & 0x0FFF;
					pc-=1;
					break;
				case 3:
					if (registers[mask(inst, 2)] == (inst & 0x00FF)) {
						pc++;
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
				case 6:
					registers[mask(inst,2)] = inst & 0x00FF;
					break;
				case 7:
					registers[mask(inst,2)] += inst & 0x00FF;
					break;
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
				case 0xA:
					indexreg = inst & 0x0FFF;
					break;
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
				case 0xD: //screen drawing
					int n = mask(inst, 4);
					bool delta = false;
					registers[0xF] = 0;
					uint8_t x = registers[mask(inst,2)] % ORIG_WIDTH;
					uint8_t y = registers[mask(inst,3)] % ORIG_HEIGHT;
					for (int iter = 0; iter < n; iter++) {
						uint8_t row = memory[indexreg + iter];
						do {
							bool bit = (bool) (row % 2);
							bool pixelVal = getPixelVal(x,y);
							if (bit && pixelVal == bit) {
								registers[0xF] = 1;
								setPixel(x,y,false);
								delta = true;
							} else if (bit && !pixelVal) {
								setPixel(x,y,true);
								delta = true;
							}
							x++;
							if (x >= ORIG_WIDTH) {
								x = 0;
								break;
							}
							row = row / 2;
						} while (row > 0);
						y++;
						if (y >= ORIG_HEIGHT) {
							y = 0;
							break;
						}
					}
					if (delta) {
						SDL_RenderClear(rend);
						SDL_Rect rect;
						rect.w = WIDTH;
						rect.h = HEIGHT;
						SDL_UpdateTexture(tex, &rect, pixels, ORIG_WIDTH * sizeof(uint16_t));
						SDL_RenderCopy(rend, tex, NULL, &rect);
						SDL_RenderPresent(rend);
					}
					break;
				default:
					if (inst) {
						fprintf(stderr,"Unknown instruction, maybe data: %x\n",inst);
					} else {
						fprintf(stderr,"Memory is zero/0\n");
					}
					pc++;
					break;
			}
			break;
	}

}
