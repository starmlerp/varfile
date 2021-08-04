#include "varfile.h"
#include <stdio.h>
#include <stdlib.h>

#define CORRELATOR '='

#define DEBUG

avf::Entry* avf::load(FILE* target){
	rewind(target);
	
	avf::Entry* object = new avf::Entry[1]; // initialize index struct
	object[0].name = NULL;
	object[0].value  = NULL;
	//TODO: implement proper parsing to detect invalid variable names (starting with numbers, including non-aphlanumerical characters etc.)
	
	enum States{// we shall represent states using enums and switch-cases
		SI, // seek for the next identifier
		ID, // identifier
		SC, // seek for the correlator
		CR, // correlator
		SB, // seek for the object
		OB, // object
		CM, // comment
		EE  // end of entry
	};;
	States state = SI;

	long line=1;
	unsigned long objnsize=0;
	unsigned long objvsize=0;
	unsigned long tracker=0;
	char* objname;
	char* objvalue;
	
	while(true){
		// we are entering an infinite loop, since the following code is going to be implemented as a state machine
		// the loop is going to be broken on a return call
		char inC = fgetc(target);
		#ifdef DEBUG
		printf("input: %c, state: %d\n", inC=='\n'?'N':inC, state);
		#endif
		if(inC==EOF)return object;
		switch(state){
			
			case SI:
				if(inC == '\n'){
					line++;// keep track of lines, will be used on displaying errors
					break;
				}
				if(inC == ' ')break;// proceed if its space
				else {
					state = ID;
					tracker = ftell(target);
				}// otherwise assume we reached an identifier
			
			case ID:
				if(objnsize==2){
					if(objname[0] == '/' && objname[1] == '/'){// if the input appears to be a start of comment
						objnsize=0;
						free(objname);
						objname=NULL;
						state = CM;// mark it as such
						break;
					}
				}
				if(inC >= 'A' && inC <= 'Z' || inC >= 'a' && inC <= 'z' || inC >= '0' && inC <= '9'){// if its an alphanumerical character
					char* holder = new char[++objnsize];// add it to the checking stream
					for(unsigned long i = 0; i < objnsize-1;i++)holder[i]=objname[i];
					holder[objnsize-1]=inC;
					if(objnsize > 1)free(objname);
					objname=holder;//dirty dirty append
					#ifdef DEBUG
					printf("object: %s (%ld)\n", objname, objnsize);
					#endif
					break;
				}

				else if(inC == ' ' || inC == CORRELATOR){//end of identifier
					state=CR;//assume we are looking for a correlator
				}

				else if(inC == '\n'){
					line++;
				}

				else {
					free(object);
					object = new avf::Entry[1];
					object[0].name = new char[1];
					object[0].name[0]=0;
					object[0].value = new char[200];
					sprintf((char*)object[0].value, "line %ld: invalid character in variable declaration\n", line);
					object[0].value[199]=NULL;
					return object;
				}
			case CR:
				if(inC == CORRELATOR){
					state = SB;
					break;
				}
				if(inC == '\n'){
					line++;
					break;
				}
				if(inC == ' '){
					break;
				}
				else {
					free(object);
					object = new avf::Entry[1];
					object[0].name = new char[1];
					object[0].name[0]=0;
					object[0].value =  new char[200];
					sprintf((char*)object[0].value, "line %ld: expected \"%c\", got \"%c\"\n", line, CORRELATOR, inC);
					object[0].value[199]=NULL;
					return object;
				}
			case SB:
				if(inC == ' '){
					break;
				}
				if(inC == '\n'){
					line++;
					break;
				}//ignore anything that could not be an object
				else{
					state = OB;
				}
			case OB:
				if(inC >= '0' && inC <= '9'){//if our value is numeric
					char* holder = new char[++objvsize];//apppend it
					for(unsigned long i = 0; i < objvsize-1;i++)holder[i]=objvalue[i];
					holder[objvsize]=inC;
					if(objvsize-1 > 0)free(objvalue);
					objvalue=holder;
					break;
				}
				else{
					free(object);
					object = new avf::Entry[1];
					object[0].name = new char[1];
					object[0].name[0]=0;
					object[0].value =  new char[200];
					sprintf((char*)object[0].value, "unexpected value for %s \n", objname);
					object[0].value[199]=NULL;
					return object;
				}
		}
	}
}

char* avf::get(FILE* target, char* key){
	
}
int avf::put(FILE* target, char* key, char* value){
	
}
//TODO: write these functions
