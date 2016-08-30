#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define TRUE 1
#define FALSE 0


typedef struct notelist_s{
	unsigned char note;
	long long int starttime;
	long long int endtime;
} notelist_t;

typedef struct tone_s {
	char * data;
	size_t len;
	int type;
} tone_t;

tone_t tones[128];

size_t listsize = 0;
size_t listplace = 0;
notelist_t * list = 0;

notelist_t * plist = 0;

int width = 8192;
int height = 132;


float tonetable[128] = {0.0};

int samplehz = 16384;

long long int samplespertick = 8;

int drumnotes[] = {0, 35, 40, 46, 0};
void createtonesfromfile(char * filename, int *notes){
	FILE *f = fopen(filename, "rb");
	int i;
	if(!f){
		for( i =0 ;notes[i]; i++){
			printf(" no file, replacing %i with nothing\n", notes[i]);
			tones[notes[i]].len = 0;
			tones[notes[i]].type = 3;
			tones[notes[i]].data = 0;
		}
		return;
	}
	fseek(f, 0, SEEK_END);
	size_t len = ftell(f);
	rewind(f);
	char *data = malloc(len);
	fread(data, 1, len, f);
	for( i =0 ;notes[i]; i++){
		tones[notes[i]].len = len;
		tones[notes[i]].data = data;
		tones[notes[i]].type = 2;
	}
	fclose(f);
}


void createtone(int note){
	if(tonetable[note] < 1.0 || tones[note].type) return;
	//find period in length of samples
	double period = (double)samplehz/tonetable[note];
	int numsamp = period;
	if(fmod(period, 1.0) > 0.5) numsamp++;
	tones[note].len = numsamp;
	tones[note].data = malloc(numsamp);
	int i;
	for( i = 0; i < numsamp; i++){
		tones[note].data[i] = sin((double) i * 2.0 * M_PI / (double) numsamp) * 10;
//		tones[note].data[i] = i >= numsamp /2 ? 42 : -42;
	}
	tones[note].type = 1;
}

void printtones(void){
	int i;
	for(i = 0; i < 128; i++){
		if(!tones[i].type)continue;
		printf("int8_t tone%i[] = {", i);
		int j;
		for(j = 0; j < tones[i].len; j++){
			printf(j < tones[i].len-1 ? "%i," : "%i", tones[i].data[j]);
		}
		printf("};\n");
	}
	printf("typedef struct tone_s {\n\tuint16_t len;\n\tint8_t * data\n}tone_t;\n");
	printf("tone_t tonetable[] = {\n\t");
	for(i = 0; i < 128; i++){
		if(tones[i].type) printf(i < 127 ? "{%i,tone%i},\n\t" : "{%i,tone%i}\n", tones[i].len, i);
		else printf(i < 127 ? "{0,0},\n\t" : "{0,0}\n");
	}
	printf("};\n");
}

int main(const int argc, const char ** argv){
	tonetable[21] = 27.5;
	tonetable[22] = 29.135;
	tonetable[23] = 30.868;
	tonetable[24] = 32.703;
	tonetable[25] = 34.648;
	tonetable[26] = 36.708;
	tonetable[27] = 38.891;
	tonetable[28] = 41.203;
	tonetable[29] = 43.654;
	tonetable[30] = 46.249;
	tonetable[31] = 48.999;
	tonetable[32] = 51.913;
	tonetable[33] = 55.000;
	tonetable[34] = 58.270;
	tonetable[35] = 61.735;
	tonetable[36] = 65.406;
	tonetable[37] = 69.296;
	tonetable[38] = 73.416;
	tonetable[39] = 77.782;
	tonetable[40] = 82.407;
	tonetable[41] = 87.307;
	tonetable[42] = 92.499;
	tonetable[43] = 97.999;
	tonetable[44] = 103.83;
	tonetable[45] = 110.00;
	tonetable[46] = 116.54;
	tonetable[47] = 123.47;
	tonetable[48] = 130.81;
	tonetable[49] = 138.59;
	tonetable[50] = 146.83;
	tonetable[51] = 155.56;
	tonetable[52] = 164.81;
	tonetable[53] = 174.61;
	tonetable[54] = 185.00;
	tonetable[55] = 196.00;
	tonetable[56] = 207.65;
	tonetable[57] = 220.00;
	tonetable[58] = 233.08;
	tonetable[59] = 246.94;
	tonetable[60] = 261.63;
	tonetable[61] = 277.18;
	tonetable[62] = 293.67;
	tonetable[63] = 311.13;
	tonetable[64] = 329.63;
	tonetable[65] = 349.23;
	tonetable[66] = 369.99;
	tonetable[67] = 392.00;
	tonetable[68] = 415.30;
	tonetable[69] = 440.00;
	tonetable[70] = 466.16;
	tonetable[71] = 493.88;
	tonetable[72] = 523.25;
	tonetable[73] = 554.37;
	tonetable[74] = 587.33;
	tonetable[75] = 622.25;
	tonetable[76] = 659.26;
	tonetable[77] = 698.46;
	tonetable[78] = 739.99;
	tonetable[79] = 783.99;
	tonetable[80] = 830.61;
	tonetable[81] = 880.00;
	tonetable[82] = 932.33;
	tonetable[83] = 987.77;
	tonetable[84] = 1046.5;
	tonetable[85] = 1108.7;
	tonetable[86] = 1174.7;
	tonetable[87] = 1244.5;
	tonetable[88] = 1318.5;
	tonetable[89] = 1396.9;
	tonetable[90] = 1480.0;
	tonetable[91] = 1568.0;
	tonetable[92] = 1661.2;
	tonetable[93] = 1760.0;
	tonetable[94] = 1864.7;
	tonetable[95] = 1975.5;
	tonetable[96] = 2093.0;
	tonetable[97] = 2217.5;
	tonetable[98] = 2394.3;
	tonetable[99] = 2489.0;
	tonetable[100] = 2637.0;
	tonetable[101] = 2793.0;
	tonetable[102] = 2960.0;
	tonetable[103] = 3136.0;
	tonetable[104] = 3322.4;
	tonetable[105] = 3520.0;
	tonetable[106] = 3729.3;
	tonetable[107] = 3951.1;
	tonetable[108] = 4186.0;
	createtonesfromfile("./drum.raw", drumnotes);

	unsigned char * data = malloc(width * height);
	memset(data, 0, width * height);
	long long int starttime =0;
	long long int endtime = 0;
	long long int maxtime = 0;
	int note=0;
	while(scanf("%*s %Ld %Ld %d", &starttime, &endtime, &note) == 3){
		if(endtime-starttime < 1) continue;
		if(listplace >= listsize){
			listsize =listplace + 32;
			list = realloc(list, listsize*sizeof(notelist_t));
		}
		createtone(note);
		list[listplace].note = note;
		list[listplace].starttime = starttime;
		list[listplace].endtime = endtime;
		listplace++;
		if(endtime > maxtime) maxtime = endtime;
//		printf("%i %i %i\n", note, starttime, endtime-starttime);
	}
	int looks = 5;
	plist = malloc(listsize*sizeof(notelist_t));
	int last = 2;
	int i;
	for(i = 0; i < listplace; i++){
		int above = 0;
		int below = 0;
		int looksstart = i - looks;
		int looksend = i + looks;
		int mykey = list[i].note;
		long long int mystart = list[i].starttime;
		long long int myend = list[i].endtime;
		if(looksstart <0) looksstart = 0;
		if(looksend > listplace) looksend = listplace;
		for(; looksstart < looksend; looksstart++){
			if(list[looksstart].note < mykey) above++;
			else if(list[looksstart].note > mykey) below++;
		}
//		printf("# above %i beloew %i\n", above, below);
		mykey = last;
		if(above > below){
			if(above > 2 * below)mykey = 0;
			else mykey =1;
		} else if(below > above){
			if(below> 2 * above)mykey = 3;
			else mykey =2;
		}
		plist[i].note = 131-mykey;
		printf("# mykey %i\n", plist[i].note);
		last = mykey;
		plist[i].starttime = mystart;
		plist[i].endtime = myend;
	}
	int j;
	int closest = 100;
	for(i = 0; i < listplace; i++){
		// search through entire list, check to make sure im not too close to another
		long long int mykey = plist[i].note;
		for(j = i+1; j < listplace; j++){
			if(plist[j].note != mykey) continue;
			if(plist[i].endtime > plist[j].starttime - closest) plist[i].endtime = plist[j].starttime - closest;
		}
	}
	for(i = 0; i < listplace; i++){
		unsigned char * line = data + width * list[i].note;
		long long int spos = (double)list[i].starttime / ((double)maxtime / (double) width);
		long long int epos = (double)list[i].endtime / ((double)maxtime / (double) width);
//		printf("# spos%i epos %i\n", spos, epos);
		unsigned char * line2 = data + width * plist[i].note;
		for(; spos < epos; spos++){
			line[spos] = 255;
		}
		spos = (double)plist[i].starttime / ((double)maxtime / (double) width);
		epos = (double)plist[i].endtime / ((double)maxtime / (double) width);
		for(; spos < epos; spos++){
			line2[spos] = 255;
		}

	}
	printtones();
	printf("typedef struct note_s {\n\tuint8_t note;\n\tuint32_t start;\n\tuint32_t end;\n};\n");
	printf("uint16_t numnotes = %i;\n", listplace);
	printf("note_t notetable[] = {\n\t");
	for(i = 0; i < listplace; i++){
		printf(i < listplace-1 ? "{%i,%i,%i},\n\t" : "{%i,%i,%i}\n", list[i].note, list[i].starttime, list[i].endtime);
	}
	printf("};\n");
	printf("uint16_t numkeys = %i;\n", listplace);
	printf("note_t keytable[] = {\n\t");
	for(i = 0; i < listplace; i++){
		printf(i < listplace-1 ? "{%i,%i,%i},\n\t" : "{%i,%i,%i}\n", 131-plist[i].note, plist[i].starttime, plist[i].endtime);
	}
	printf("};\n");

	FILE *o = fopen(argv[1], "wb");

	//now go through and make da musix1
	long long int curnote = 0;
	long long unsigned int chi;
	for(chi = 0; chi < maxtime*samplespertick && curnote < listplace; chi++){
		float accum = 0.0;
		//run through notes until i get one that starts after me
		//slap sample in there
		long long int z, ki;
		for(ki = 0, z = curnote; ki < 255 &&curnote < listplace && list[z].starttime * samplespertick < chi; z++, ki++){
			tone_t t = tones[list[z].note];
			if(!t.type) continue;
			int dstart = chi - list[z].starttime * samplespertick;
			int dend = list[z].endtime * samplespertick - chi;
			float atten = dend < 10 ? dend * 0.1 : 1.0;
			float atten2 = dstart < 10 ? dstart * 0.1 : 1.0;
//			float atten = 1.0;
			accum += t.data[dstart % t.len];// * atten;// * atten2;
//			accum += t.data[chi% t.len];
		}
		//output cur
//		if(ki) accum /= (double)ki;
//		else accum = 0.0;
		if(accum > 127.0) accum = 127.0;
		else if (accum < -128.0) accum = -128.0;
		char data = accum;
		fwrite(&data, 1, 1, o);
		//update cur
		for(; curnote < listplace && list[curnote].endtime*samplespertick < chi; curnote++);
	}
	printf("chi was %i\n", chi);
	fclose(o);

/*
	printf("P3 %i %i 255\n", width, height);
	for(i = 0; i < height; i++){
		unsigned char * line = data + width * i;
		for(j = 0; j < width; j++){
			printf("%i %i %i\t", line[j], line[j], line[j]);
		}
		printf("\n");
	}
*/
	return 0;
}
