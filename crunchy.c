#include <stdio.h>
#include <stdlib.h>
#include <math.h>



int main(const int argc, const char ** argv){
	FILE *f = fopen(argv[1], "rb");
	if(!f){
		printf("lol\n");
		return 1;
	}
	fseek(f, 0, SEEK_END);
	size_t fsize = ftell(f);
	rewind(f);
	short *data = malloc(fsize);
	fread(data, 1, fsize, f);
	fclose(f);

	int smallest = data[0];
	int largest = data[0];
	int i;
	for(i = 0; i < fsize/2; i++){
		if(smallest > data[i]) smallest = data[i];
		if(largest  < data[i]) largest = data[i];
	}
	printf("Smallest %i largest %i\n", smallest, largest);
	double biggest = fabs(smallest);
	if(fabs(largest) > biggest) biggest = fabs(largest);
	double wanter = (double)32767/(double)biggest;
	for(i = 0; i < fsize/2; i++){
//		double tmp = (double)data[i] *wanter;
//		if(tmp > 32767.0){
//			printf("over %f\n", tmp);
//			tmp = 32767.0;
//		}
//		data[i] = tmp;
//		data[i] = data[i] & 0xF0<<8;
		data[i] = (((int)data[i]>>12) << 12);
	}
	FILE *o = fopen(argv[2], "wb");
	fwrite(data, fsize, 1, o);
	fclose(o);
	free(data);

	return 0;
}
