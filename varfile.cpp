//#define _DATANODE_H_ //dirty dirty fix
#include "varfile.h"
#include <stdio.h>

avf::Index* avf::index(FILE* target, short wlen){
	rewind(target);
	unsigned long elen = 0;
	while(!feof(target)){
		//record the number of times we encounter ";", essentially counting the number of entries we are dealing with
		//TODO: make proper parsing
		if(fgetc(target)==';')elen++;
	}
	#ifdef DEBUG 
		printf("%l", elen);
	#endif
	avf::Index* index = new avf::Index[1]; //initialize index struct
	index[0].word = 0;
	rewind(target);
	if(wlen > 0){
		for(unsigned long i = 0; i < elen; i++){
			char w[wlen];
			char neutralizer = 1;
			for(short j = 0; j < wlen; j++){
				w[j]=fgetc(target);
				netralizer *= w[j] >= 97 || w[j] <= 122;
				//^checks if the fetched character is a lowercase letter
				//ASCII: a = 97, z = 122
				//otherwise stop recording from stream, by making every next entry NULL
				w[j]*=neutralizer;

			}
			for(unsigned long j = 0; index[j].word; j++){ //append through the index...
				short isEntry = 1;
				for(short k = 0; k < wlen; k++){
					isEntry *= w[k] == index[j].word[k];
				}
				if(isEntry){ //if the word from current entry matches index entry
					
				}
			}
		}
	}
}

char* avf::get(FILE* target, char* key){
	
}

char* avf::get(FILE* target, char* key, avf::Index* index){
	
}

int avf::put(FILE* target, char* key, char* value){
	
}

int avf::put(FILE* target, char* key, char* value, avf::Index* index){
	
}
//TODO: write these functions
