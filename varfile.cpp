#include "varfile.h"
#include <stdio.h>
#include <stdlib.h>

avf::Index* avf::index(FILE* target, short wlen){
	rewind(target);
		
	avf::Index* index = new avf::Index[1]; // initialize index struct
	index[0].word = NULL;
	index[0].pos  = NULL;
	//TODO: implement proper parsing to detect invalid variable names (starting with numbers, including non-aphlanumerical characters etc.)
	
	if(wlen > 0){
		enum States{// we shall represent states using enums and switch-cases
			SK, // seek for the next identifier
			ID, // identifier
			CR, // correlator 
			OB, // object
			CM  // comment
		};;
		States state = SK;
		long line=0;
		unsigned long insize=0;
		unsigned long tracker=0;
		char* instr;

		while(true){
			// we are entering an infinite loop, since the following code is going to be implemented as a state machine
			// the loop is going to be broken on a return call
			char inC = fgetc(target);
			switch(state){
				

				case SK:
					if(inC == '\n'){
						line++;// keep track of lines, will be used on displaying errors
						break;
					}
					if(inC == ' ')break;// proceed if its space
					else {
						state=ID;
						tracker = ftell(target);
					}// otherwise assume we reached an identifier
				

				case ID:
					if(insize==2){
						if(instr[0]=='/' && instr[1]=='/'){// if the input appears to be a start of comment
							state=CM;// mark it as such
							break;
						}
					}
					if(inC >= 'A' && inC <= 'Z' || inC >= 'a' && inC <= 'z' || inC >= '0' && inC <= '9'){// if its an alphanumerical character
						char* holder = new char[++insize];// add it to the checking stream
						for(unsigned long i = 0; i < insize-1;i++)holder[i]=instr[i];
						holder[insize]=inC;
						if(insize-1 > 0)free(instr);
						instr=holder;//dirty dirty append
					}
					else {
						free(index);
						index = new avf::Index[1];
						index[0].word = new char[1];
						index[0].word[0]=0;
						index[0].pos = (unsigned long*) new char[200];
						sprintf((char*)index[0].pos, "line %ld: invalid character in variable declaration\n", line);
						return index;
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
