#include <stdint.h>
#define STACK_SIZE 16

uint8_t registers[16];

uint16_t stack[STACK_SIZE];
int stacklast = -1;

uint16_t indexreg = 0x200;
uint8_t memory[4096];

uint16_t pc = 0x200;
bool running = true;

typedef enum {
	PSH,
	ADD,
	POP,
	SET,
	HLT
} InstructionSet;

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

void add(void) {

	int last = pop();
	int secondlast = pop();

	int result = last + secondlast;
	push(result);

}

void set(void);
