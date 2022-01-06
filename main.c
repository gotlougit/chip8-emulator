#include <stdio.h>
#include <stdbool.h>
#include "structures.h"

const int program[] = {

	PSH, 5,
	PSH, 6,
	ADD,
	POP,
	HLT
};

int fetch() {
	pc++;
	return program[pc-1];
}

void eval(int inst) {
	
	switch (inst) {
		default:
			break;
		case PSH:
			push(fetch());
			break;
		case ADD:
			add();
			break;
		case SET:
			break;
		case POP:
			pop();
			break;
		case HLT:
			running = false;
			break;
	
	}

}

int main(void) {
	
	initmemory();

	while (running) {
		eval(fetch());
	}
	return 0;

}
