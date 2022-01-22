#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "features.h"

int ipc = 0;

uint8_t registers[16];

uint8_t delayTimer = 100;
uint8_t soundTimer = 100;

int stacklast = -1;
uint16_t stack[STACK_SIZE];

uint16_t indexreg = 0x200;
uint16_t memory[4096];
uint16_t pc = 0x200;

bool running = true;

int * returnROM(char *loc) {
	
	FILE *fp;

	fp = fopen(loc,"rb");
	
	if (fp != NULL) {
		int c;
		fseek(fp, 0L, SEEK_END);
		int size = ftell(fp);
		rewind(fp);

		int *out = malloc(size*sizeof(int)+1);
		out[0] = size;
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
		return 0;
	}


}

void loadROM(int *rom) {
	
	for (int i = 1; i < rom[0]; i++) {
		memory[0x200 + i - 1] = rom[i];
		fprintf(stderr,"Loaded, memory loc %x value is %04x\n",0x200+i-1,memory[0x200+i-1]);
	}
	free(rom);

}

void loadfont(void) {
	
	int font[80] = {
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

	for (int i = 0x50; i < 0xA0; i++) {
		memory[i] = font[i-0x50];
	}

}

void initmemory(void) {
	
	for (int i = 0x0; i < 0x50; i++) {
		memory[i] = 0;
	}
	loadfont();
	for (int i = 0xA0; i < 0x200; i++) {
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

int mask(uint16_t num, int digit) {

	digit = 4 - digit;
	int x = 0xF * (int) pow(16,digit);
	num = num & x;
	x = 0x1 * (int) pow(16,digit);
	num = (int) num / x;
	return num;

}

int fetch(void) {

	fprintf(stderr,"memory: 0x%x\n",pc);
	pc+=1;
	return memory[pc-1];
	
}

void eval(int inst) {

	fprintf(stderr,"inst: %x ",inst);
	switch (inst) {
		default:
			int firstnibble = mask(inst, 1);
			switch (firstnibble) {
				default:
					if (inst) {
						fprintf(stderr,"Unknown instruction %x\n",inst);
					} else {
						fprintf(stderr,"Maybe uninitialized memory: %x\n",inst);
					}
					break;
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
				case 10:
					indexreg = inst & 0x0FFF;
					break;
				case 11:
					if (BXNN) {
						pc = (inst & 0x0FFF) + registers[mask(inst,2)];
					} else {
						pc = (inst & 0x0FFF) + registers[0];
					}
					break;
				case 12:
					registers[mask(inst,2)] = (inst & 0xFF) && (rand());		
					break;
				case 13:
					printf("Display\n");
					break;
			}
			break;
		case 0xE0:
			printf("\033c");
			printf("Display func for clear screen\n");
			break;
		case 0xEE:
			pc = pop();
			break;
	
	}

}
