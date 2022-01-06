#include <stdint.h>
#define STACK_SIZE 16

typedef enum {
	V0,V1,V2,V3,V4,V5,V6,
	V7,V8,V9,VA,VB,VC,VD,
	VE,VF
} Registers;

uint16_t stack[STACK_SIZE];
int stacklast = -1;

int ip = 0;
bool running = true;

typedef enum {
	PSH,
	ADD,
	POP,
	SET,
	HLT
} InstructionSet;

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
