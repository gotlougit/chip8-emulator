#include <stdio.h>
#include <stdint.h>
#include "display.h"

//implemented as macros to clean up code

#define getOPCODE(inst) (inst / 0x1000)
#define getX(inst) ((inst / 0x100) % 0x10)
#define getY(inst) ((inst / 0x10) % 0x10)
#define getN(inst) (inst % 0x10)
#define getNN(Y,N) ((0x10*Y) + N)
#define getNNN(X,NN) ((0x100*X) + NN)

//important variables that implement most of the hardware of CHIP-8

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
int key = 0;

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

//load ROM from disk

void loadROM(char *loc) {
	
	FILE *fp;
	fp = fopen(loc,"rb");
	
	if (fp != NULL) {
		int i = 0, c;
		while ((c = fgetc(fp)) != EOF) {
			memory[INIT_MEM + i] = c;
			fprintf(LOGFILE, "Loaded memory address %x, value is %x\n",INIT_MEM + i, c);
			i++;
		}
	}
	fclose(fp);

}

//initialize memory by setting everything to zero and then loading font

void initmemory(void) {

	memset(memory, 0, MEM_SIZE - 1);
	for (int i = 0x50; i < 0xA0; i++) {
		memory[i] = font[i-0x50];
	}

}

void updateTimers(void) {

	if (delayTimer > 0) {
		delayTimer--;
	}
	if (soundTimer > 0) {
		//TODO: make beep sound
		soundTimer--;
	}

	fprintf(LOGFILE, "Delay timer: %d, Sound timer: %d\n",delayTimer,soundTimer);

}

//stack management

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

void isUnknownInst(int inst) {

	if (inst) {
		fprintf(LOGFILE,"Unknown instruction, maybe data: %x\n",inst);
	} else {
		fprintf(LOGFILE,"Memory is zero/0\n");
	}

}

//fetch next instruction and prevent overflows

int fetch(void) {

	fprintf(LOGFILE,"memory: 0x%x\n",pc);
	pc+=2;
	if (pc <= MEM_SIZE) {
		return (0x100 * memory[pc]) + memory[pc+1];
	} else {
		fprintf(LOGFILE, "Tried to access out of bounds memory, setting back to %x\n", INIT_MEM);
		reset++;
		if (RESET_THRESHOLD && reset > RESET_THRESHOLD) {
			fprintf(LOGFILE, "Threshold reached, exiting process\n");
			running = false;
		}
		pc = INIT_MEM;
		fetch();
	}
	
}

//interpretes each instruction

void eval(int inst, SDL_Renderer *rend, SDL_Texture *tex) {

	fprintf(LOGFILE,"inst: %x ",inst);

	switch (inst) {
		case 0xE0:
			memset(pixels, 0, ORIG_PIXEL_COUNT - 1);
			SDL_RenderClear(rend);
			SDL_RenderPresent(rend);
			break;
		case 0xEE:
			pc = pop();
			break;
		default:
			int OPCODE = getOPCODE(inst);
			int X = getX(inst);
			int Y = getY(inst);
			int N = getN(inst);
			int NN = getNN(Y,N);
			int NNN = getNNN(X,NN);

			switch (OPCODE) {
				case 2: //so it falls through to case 1
					push(pc);
				case 1: //jump
					pc = NNN - 2;
					break;
				case 3: //skip if equal
					pc = (registers[X] == NN) ? (pc + 2) : pc;
					break;
				case 4: //skip if not equal
					pc = (registers[X] != NN) ? (pc + 2) : pc;
					break;
				case 5: //skip if equal
					pc = (registers[X] == registers[Y]) ? (pc + 2) : pc;
					break;
				case 6: //set
					registers[X] = NN;
					break;
				case 7: //add
					registers[X] += NN;
					break;
				case 8:
					switch (N) {
						case 0: //set equal to
							registers[X] = registers[Y];
							break;
						case 1: //bitwise OR
							registers[X] = registers[X] | registers[Y];
							break;
						case 2: //bitwise AND
							registers[X] = registers[X] & registers[Y];
							break;
						case 3: //logical XOR
							registers[X] = registers[X] ^ registers[Y];
							break;
						case 4: //add
							int sum = registers[X] + registers[Y];
							registers[0xF] = (sum > 0xFF)?1:0;
							registers[X] = sum;
							break;
						case 5: //subtract y from x
							registers[0xF] = (registers[X] > registers[Y]) ? 1 : 0;
							registers[X] -= registers[Y];
							break;
						case 6: //shift right
							if (EIGHTXY_VY) {
								registers[X] = registers[Y];
							}
							registers[0xF] = registers[X] & 1;
							registers[X] = registers[X] >> 1;
							break;
						case 7: //subtract x from y
							registers[0xF] = (registers[Y] > registers[X]) ? 1 : 0;
							registers[X] = registers[Y] - registers[X];
							break;
						case 0xE: //shift left
							if (EIGHTXY_VY) {
								registers[X] = registers[Y];
							}
							registers[0xF] = (registers[X] >> 7) & 1;
							registers[X] = registers[X] << 1;

							break;

					}
					break;
				case 9: //skip if not equal
					pc = (registers[X] != registers[Y]) ? (pc+2) : pc;
					break;
				case 0xA: //set indexreg
					indexreg = NNN;
					fprintf(LOGFILE, "indexreg set to: %x\n", indexreg);
					break;
				case 0xB: //jump with offset
					//pc = (BXNN) ? (NNN + registers[X]) : (NNN + registers[0]);
					pc = NNN + registers[0];
					break;
				case 0xC: //RNG
					registers[X] = (NN) && (rand());		
					break;
				case 0xD: //screen drawing
					bool hasScreenChanged = false;
					registers[0xF] = 0;
					int y = registers[Y] % ORIG_HEIGHT;
					for (int iter = 0; iter < N; iter++) {
						int x = registers[X] % ORIG_WIDTH;
						uint8_t row = memory[indexreg + iter];
						int power = 7;
						while (row > 0) {
							bool bit = (bool) (row >> power);
							bool pixelVal = getPixelVal(x,y);
							bool newVal = bit ^ pixelVal;
							if (newVal != pixelVal) {
								hasScreenChanged = true;
							}
							registers[0xF] = (!(newVal) && pixelVal);
							setPixel(x,y,newVal);
							row = row % ((int) pow(2,power));
							power--;
							x++;
							if (x > ORIG_WIDTH) {
								x = 0;
								break;
							}
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
				case 0xE: //key pressing related
					switch (NN) {
						case 0x9E:
							pc = (registers[X] == (key - 1)) ? (pc + 2) : pc;
							break;
						case 0xA1:
							pc = (registers[X] != (key - 1)) ? (pc + 2) : pc;
							break;
						default:
							isUnknownInst(inst); //unknown instruction
							break;
					}
					break;
				case 0xF: //misc instructions
					switch (NN) {
						case 7:
							registers[X] = delayTimer;
							break;
						//input related
						case 0xA:
							registers[X] = (key) ? (key - 1) : registers[X];
							pc = (key) ? (pc - 2) : pc;
							break;
						//timer related
						case 0xF:
							delayTimer = registers[X];	
							break;
						case 0x18:
							soundTimer = registers[X];
							break;
						case 0x1E:
							indexreg += registers[X];
							break;
						case 0x29:
							indexreg = registers[X];
							break;
						//convert to decimal
						case 0x33:
							memory[indexreg + 2] = registers[X] % 10;
							memory[indexreg+1] = (registers[X] / 10) % 10;	
							memory[indexreg] = (registers[X] / 100);
							break;
						//memory load and store
						case 0x55:
							for (int iter = 0; iter <= X; iter++) {
								if (FX55_FX65_INCREMENT) {
									memory[indexreg] = registers[iter];
									indexreg++;
								} else {
									memory[indexreg + iter] = registers[iter];
								}
							}
							break;
						case 0x65:
							for (int iter = 0; iter <= X; iter++) {
								if (FX55_FX65_INCREMENT) {
									registers[iter] = memory[indexreg];
									indexreg++;
								} else {
									registers[iter] = memory[indexreg + iter];
								}
							}
							break;
						default: //unknown instruction
							isUnknownInst(inst);
							break;
					}
					break;
				default: //means this is unknown
					isUnknownInst(inst);	
					break;
			}
			break;

	}

}
