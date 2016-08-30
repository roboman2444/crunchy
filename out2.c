#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TRUE 1
#define FALSE 0


typedef struct notelist_s{
	unsigned char note;
	long long int starttime;
	long long int endtime;
} notelist_t;

size_t listsize = 0;
size_t listplace = 0;
notelist_t * list = 0;

notelist_t * plist = 0;

int width = 8192;
int height = 132;

int main(const int argc, const char ** argv){
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
		printf("# above %i beloew %i\n", above, below);
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
		printf("# spos%i epos %i\n", spos, epos);
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
	printf("P3 %i %i 255\n", width, height);
	for(i = 0; i < height; i++){
		unsigned char * line = data + width * i;
		for(j = 0; j < width; j++){
			printf("%i %i %i\t", line[j], line[j], line[j]);
		}
		printf("\n");
	}
	return 0;
}
