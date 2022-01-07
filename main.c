#include <stdio.h>
#include <stdbool.h>
#include "structures.h"
#include "loader.h"

int fetch(int size) {
	pc+=1;
	if (pc == 0x200 + size - 1) {
		running = false;
		printf("\n");
	}
	return memory[pc-1];
	
}

void eval(int inst) {
	
	printf("%x ",inst);
	
	/*
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
	*/

}

int main(void) {
	
	initmemory();
	int *out = returnROM("ibm.ch8");
	int size = out[0];
	for (int i = 1; i < size; i++) {
		memory[0x200 + i - 1] = out[i];
	}
	free(out);
	while (running) {
		eval(fetch(size));
	}
	return 0;

}
