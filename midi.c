#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define TRUE 1
#define FALSE 0

//returns TRUE if both strings are equal, false otherwise. tests only first n characters
//suitable replacement for !strcmp() if you only want to check if they are the same
//unsigned int because strings might be >2gb. Call the string length police i dont care.
int string_testEqualN(const char * one, const char * two, const unsigned int n){
        if(!one || !two) return FALSE;
        char * m1 = (char *)one;
        char * m2 = (char *)two;
        unsigned int i;
        //i dont test both in the while because checking weather they are equal does it for me
        for(i = 0; i < n && *m1; i++) if(*m1++ != *m2++) return FALSE;
        return TRUE;
}

typedef struct chunkheader_s{
	char type[4];
	int32_t size;
	uint8_t data[0];
} chunkheader_t;

typedef struct chunk_typeheader_s {
	int16_t format;
	int16_t tracks;
	int16_t division;
} chunk_typeheader_t;

typedef struct event_meta_s {
	uint8_t type;
	uint8_t data[0];
} event_meta_t;


#define tole(a,b) (((uint8_t *) &(a))[0] = ((uint8_t *) &(b))[3] ); (((uint8_t *) &(a))[1] = ((uint8_t *) &(b))[2] ); (((uint8_t *) &(a))[2] = ((uint8_t *) &(b))[1] ); (((uint8_t *) &(a))[3] = ((uint8_t *) &(b))[0] );
#define tole2(a,b) (((uint8_t *) &(a))[0] = ((uint8_t *) &(b))[1] ); (((uint8_t *) &(a))[1] = ((uint8_t *) &(b))[0] );


int getvarint(int *out, uint8_t *in){
	int i;
	int toout = 0;
	for(i = 0; i < 5;){
		uint8_t det = in[i];
		toout |= (det & 0x7F) << (7 * i);
		i++;
		if(!(det &0x80)) break;
	}
	*out = toout;
//	printf("varint %i\n", i);
	return i;
}


typedef struct eventqueue_s {
	struct eventqueue_s * next;
	double time;
	uint8_t key;
	uint8_t vel;
	uint8_t type;
} eventqueue_t;

int eventinsert(eventqueue_t **q, double time , uint8_t key, uint8_t vel, uint8_t type){
	eventqueue_t *n = malloc(sizeof(eventqueue_t));
	n->time = time;
	n->key = key;
	n->vel = vel;
	n->type = type;
	n->next = 0;
	eventqueue_t *bitter;
	if(!*q){
		*q = n;
		return 0;
	}
	for(bitter = *q; bitter->next &&  time > bitter->next->time; bitter = bitter->next);
	n->next = bitter->next;
	bitter->next = n;
	return 1;
}

int eventfree(eventqueue_t *q){
	eventqueue_t *s, *t;
	for(s = t = q; s; s= t){
		t= s->next;
		free(s);
	}
	return 0;
}




int eventwalk(eventqueue_t *q){
char keys[256] = {0};
	eventqueue_t *s;
	for(s = q; s; s= s->next){
		if((s->type & 0xF0) == 0x80){
			printf("key %i up at %f with vel %i\n", s->key, s->time, s->vel);
			keys[s->key] = 0;
		} else if((s->type & 0xF0) == 0x90){
			if(s->vel){
				printf("key %i down at %f with vel %i\n", s->key, s->time, s->vel);
				keys[s->key] = 1;
			} else {
				printf("key %i up at %f\n", s->key, s->time);
				keys[s->key] = 0;
			}
		} else printf("unknown event 0x%02X at time %f\n", s->type, s->time);
		int i;
//		for(i = 0; i < 64; i++)putc(keys[i%64] ? '|' : ' ', stdout);
//		putc('\n', stdout);
	}
	return 0;
}
int eventtext(eventqueue_t *q){
	eventqueue_t *s;
	for(s = q; s; s= s->next){
		if((s->type & 0xF0) == 0x90 && s->vel){
			eventqueue_t *search;
			for(search = s; search->next && !((search->type &0xF0 == 0x80 || !search->vel) && search->key == s->key); search = search->next);
			printf("Note\t%f\t%f\t%i\n",s->time, search->time, s->key);
		}
//		for(i = 0; i < 64; i++)putc(keys[i%64] ? '|' : ' ', stdout);
//		putc('\n', stdout);
	}
	return 0;
}
int eventdraw(eventqueue_t *q, const char * filename){
	size_t len = strlen(filename)+1;
	char * fname2 = malloc(len + 4);
	sprintf(fname2, "%s.ppm", filename);
	FILE *f = fopen(fname2, "w");
	free(fname2);
	int i;
	char keys[256] = {0};
	short keyplace[256] = {-1};
	eventqueue_t *s;
	for(i = 0, s = q; s; s= s->next, i++){
		keys[s->key] = 1;
	}
	int k, j;
	for(k = j = 0; j < 256; j++){
		if(keys[j]) keyplace[k++] = j;
	}
	fprintf(f, "P3 %i %i 255\n", k, i);
	memset(keys, 0, 256);
	for(s = q; s; s= s->next){
//	memset(keys, 0, 256);
		if((s->type & 0xF0) == 0x80) keys[s->key] = 0;
		else if((s->type & 0xF0) == 0x90){
			if(s->vel) keys[s->key] = 1;
			else keys[s->key] = 0;
		}
		int i;
		for(i = 0; i < k; i++){
			fprintf(f, keys[keyplace[i]] ? "255 255 255\t" : "0 0 0\t");
		}
		fprintf(f, "\n");
//		putc('\n', stdout);
	}
	return 0;
}






int main(const int argc, const char ** argv){
	int i;
	for(i = 1; i < argc; i++){
		eventqueue_t *q = 0;
		chunkheader_t **tracks = 0;
		int ntracks = 0;
		int ctracks = 0;
		FILE *f = fopen(argv[i], "rb");
		if(!f){
			printf("lol\n");
			return 1;
		}
		fseek(f, 0, SEEK_END);
		size_t fsize = ftell(f);
		rewind(f);
		uint8_t *data = malloc(fsize);
		fread(data, 1, fsize, f);
		fclose(f);
		uint8_t * place = data;
		printf("file %s fsize is %i\n", argv[i], fsize);
		chunk_typeheader_t fixed;
		for(place = data; (place - data) < fsize; ){
	//		printf("place is %i past\n", place - data);
			chunkheader_t *chunk = (chunkheader_t *)place;
			int32_t size;
			tole(size, chunk->size);
			place = (uint8_t *)chunk->data + size;
			printf("got chunk type %c%c%c%c size %i\n", chunk->type[0], chunk->type[1], chunk->type[2], chunk->type[3], size);
			if(string_testEqualN(chunk->type, "MThd", 4)){
				printf("got a header chunk\n");
				chunk_typeheader_t *detter = (chunk_typeheader_t *) chunk->data;
				tole2(fixed.format, detter->format);
				tole2(fixed.tracks, detter->tracks);
				tole2(fixed.division, detter->division);
				printf("header chunk %i format, %i tracks, %i division\n", fixed.format, fixed.tracks, fixed.division);
				if(tracks) free(tracks);
				ntracks = fixed.tracks;
				tracks = malloc(ntracks * sizeof(chunkheader_t *));

			} else if(string_testEqualN(chunk->type, "MTrk", 4)){
				if(!tracks || ctracks >= ntracks){
					printf("track error!\n");
				}
				tracks[ctracks] = chunk;
				ctracks++;
			}
		}
		int j;
		for(j = 0; j < ntracks; j++){
			chunkheader_t *ch = tracks[j];
			int size;
			tole(size, ch->size);
			printf("track %i size %i\n", j, size);
			uint8_t lasttype= 0;
			unsigned int time = 0;
			char keys[256] = {0};
			for(place = ch->data; (place - ch->data) < size; ){
				int dtime;
				int dsize;
				place+= getvarint(&dtime, place);
				uint8_t type =*place;
				if(type & 0x80)place++;
				else{
					printf("Running status prev 0x%02X ", type);
					type = lasttype;
				}
				lasttype = type;
				printf("dtime is %i, type is 0x%02X\n", dtime, type);
				uint8_t postype = type & 0xF0;
				uint8_t retype = type & 0x0F;
				uint8_t key = 0, vel =0;
				switch(postype){
					case 0x80:{
						key = place[0];// & 0x7F;
						vel = place[1];// & 0x7F;
						place+=2;
						printf("key released, %i, %i\n", key, vel);
					} break;
					case 0x90: {
						key = place[0];// & 0x7F;
						vel = place[1];// & 0x7F;
						place+=2;
						printf("key pressed, %i, %i\n", key, vel);
					} break;
					case 0xA0:
						place+=2;
					break;
					case 0xB0:
						place+=2;
					break;
					case 0xC0:
						place+=1;
					break;
					case 0xD0:
						place+=2;
					break;
					case 0xE0:
						place+=2;
					break;
					case 0xF0: if(type == 0xFF){ //sysex events, todo
						uint8_t metatype = *place;
						place++;
						place+= getvarint(&dsize, place);
						printf("metatype is 0x%02X, size is %i\n", metatype, dsize);
						if(metatype == 0x51){
							int ntemp = place[1] | place[2] << 8 | place[3] << 16;
							printf("NEW TEMPO %i\n", ntemp);
						}
						place+= dsize;
					} else {
						int mysize;
						place+= getvarint(&mysize, place);
						printf("sysex size is %i\n", mysize);
						place+= mysize;
//							printf("FUCKSYS\n");
//							place += size;
					} break;
					default:
						printf("FUCK\n");
						place += size;
					break;
				}
				eventinsert(&q, time, key, vel, type);
				keys[key] = 1;
				time+= dtime;
			}
			printf("end at %i\n", time);
			int k;
			for(k = 0; k < 256; k++){
					if(keys[k])eventinsert(&q, time, k, 40, 0x80);
			}
		}
		//time to run through tracks and do shit wit em
		if(tracks) free(tracks);
		if(data) free(data);
		eventwalk(q);
		eventdraw(q, argv[i]);
		eventtext(q);
		eventfree(q);
	}
	return 0;
}
