#include <time.h>
#include "structures.h"

int main(void) {

	srand(time(NULL));
	int *out = returnROM("ibm.ch8");
	initmemory();
	loadROM(out);
	while (running) {
		time_t beginningTime = time(NULL);
		for (;ipc < INST_PER_SEC; ipc++) {
			if (!(ipc % 60)) {
				updateTimers();
			}
			eval(fetch());
		}
		if (time(NULL) - beginningTime >= 1) {
			ipc = 0;
		}
	}
	return 0;

}
