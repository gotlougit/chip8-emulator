#include <stdio.h>
#include <stdlib.h>

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
			out[i] = c;
			i++;
		}
		return out;
	}

	fclose(fp);

}
