#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(const int argc, const char ** argv){
	int wavesize = atoi(argv[1]);
	int repeat = atoi(argv[2]);
	char *samples = malloc(wavesize);
	int i;
	for(i = 0; i < wavesize; i++){
//		samples[i] = sin((double)i * 2.0 * M_PI / (double)wavesize) * 127;
//		samples[i] = i > wavesize/2 ? 127 : -128;
	}
	FILE *o = fopen(argv[3], "wb");
	for(i = 0; i < repeat; i++){
		fwrite(samples, wavesize, 1, o);
	}
	fclose(o);
	free(samples);
	return 0;
}
