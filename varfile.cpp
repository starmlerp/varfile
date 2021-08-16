#include "varfile.h"
#include <stdio.h>
#include <stdlib.h>

#define CORRELATOR '='
#define TERMINATOR ';'
#define QUOTE '\"'

avf::Entry* avf::load(FILE* target){
	rewind(target);
	
	avf::Entry* object = NULL;
	//TODO: implement proper parsing to detect invalid variable names (starting with numbers, including non-aphlanumerical characters etc.)
	
	enum States{// we shall represent states using enums and switch-cases
		EE,  // end of entry
		SI, // seek for the next identifier
		ID, // identifier
		SC, // seek for the correlator
		CR, // correlator
		SV, // seek for the value
		OV, // value
		QU, // quote
		QE, // escaped character in quotes
		RF, // object reference
		CM // comment
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
		char inC;
		if(state)inC = fgetc(target);
		#ifdef DEBUG
		printf("input: %c, state: %d\n", inC=='\n'?'N':inC, state);
		fflush(stdout);
		#endif
		if(inC=='\n')line++;//keep track of lines, will be used to display errors
		if(inC==EOF){
			if(state==SI)return object;
			else{
				free(object);
				object = new avf::Entry[1];
				object[0].name = new char[1];
				object[0].name[0]=0;
				object[0].value = new char[200];
				sprintf((char*)object[0].value, "line %ld: unexpected end-of-file\n", line);
				object[0].value[199]='\0';
				return object;
			}
		}
		switch(state){
			
			case SI:
				if(inC == '\n')break;
				if(inC == ' ')break;// proceed if its space
				else {
					state = ID;
					tracker = ftell(target);
				}// otherwise assume we reached an identifier
			
			case ID:
				if(objnsize==2){
					//TODO: following condition is impossible to meet. fix that
					if(objname[0] == '/' && objname[1] == '/'){// if the input appears to be a start of comment
						objnsize=0;
						free(objname);
						objname=NULL;
						state = CM;// mark it as such
						break;
					}
				}
				if(inC >= 'A' && inC <= 'Z' || inC >= 'a' && inC <= 'z' || inC >= '0' && inC <= '9'){// if its an alphanumerical character
					char* holder = new char[++objnsize+1];// add it to the checking stream
					//holder is an additional character long in order to hold null terminator
					for(unsigned long i = 0; i < objnsize-1;i++)holder[i]=objname[i];
					holder[objnsize-1]=inC;
					if(objnsize > 1)free(objname);
					objname=holder;//dirty dirty append
					break;
				}

				else if(inC == ' ' || inC == CORRELATOR){//end of identifier
					objname[objnsize]='\0';
					state=CR;//assume we are looking for a correlator
				}

				else {
					free(object);
					object = new avf::Entry[1];
					object[0].name = new char[1];
					object[0].name[0]=0;
					object[0].value = new char[200];
					sprintf((char*)object[0].value, "line %ld: invalid character in variable declaration\n", line);
					object[0].value[199]='\0';
					return object;
				}
			case CR:
				if(inC == CORRELATOR){
					state = SV;
					break;
				}

				if(inC == '\n')break;
				if(inC == ' ')break;
				
				else {
					free(object);
					object = new avf::Entry[1];
					object[0].name = new char[1];
					object[0].name[0]=0;
					object[0].value =  new char[200];
					sprintf((char*)object[0].value, "line %ld: expected \"%c\", got \"%c\"\n", line, CORRELATOR, inC);
					object[0].value[199]='\0';
					return object;
				}
			case SV:
				if(inC == ' ' || inC == '\n')break; // ignore anything that could not be an object
				else if(inC == '\"'){
					state = QU;
					break;
				}	
				else state = OV;

			case OV:
				if(inC == TERMINATOR){
					objvalue [objvsize]='\0';
					state=EE;
					break;
				}
				else if(inC >= '0' && inC <= '9'){ // if our value is numeric
					char* holder = new char[++objvsize+1]; // apppend it
					for(unsigned long i = 0; i < objvsize-1;i++)holder[i]=objvalue[i];
					holder[objvsize-1]=inC;
					if(objvsize > 1)free(objvalue);
					objvalue=holder;
					break;
				}

				else if(inC == ' ' || inC == '\n'){
					break;
				}
				else{
					free(object);
					object = new avf::Entry[1];
					object[0].name = new char[1];
					object[0].name[0] = 0;
					object[0].value =  new char[200];
					sprintf((char*)object[0].value, "line %ld: unexpected value for %s \n", line, objname);
					object[0].value[199]='\0';
					return object;
				}
			case QU:
				if(inC == QUOTE){ // if second quotation mark is found
					state = OV; // switch to end state: we know nothing more is here
					break; // TODO: wacky. could cause bugs. implement seek to end state maybe???
				}
				if(inC == '\\'){ // if escape sign is encountered
					state = QE;
					printf("state QU: %c\n", inC);
					fflush(stdout);
					break;
				}
				else {
					char* holder = new char[++objvsize+1]; // apppend it
					for(unsigned long i = 0; i < objvsize-1;i++)holder[i]=objvalue[i];
					holder[objvsize-1]=inC;
					if(objvsize > 1)free(objvalue);
					objvalue=holder;
					break;
				}
			case QE:{
				printf("state QE: %c\n", inC);
				fflush(stdout);
				char* holder = new char[++objvsize+1]; // apppend it
				for(unsigned long i = 0; i < objvsize-1;i++)holder[i]=objvalue[i];
				holder[objvsize-1]=inC;
				if(objvsize > 1)free(objvalue);
				objvalue=holder;
				state = QU;
				break;
			}
			case EE:
				if(object){
					unsigned long outsize;
					for(outsize = 0; object[outsize].name; ++outsize);
					avf::Entry* holder = new avf::Entry[++outsize+1];
					for(unsigned long i = 0; i < outsize; i++){
						holder[i].name = object[i].name;
						holder[i].value = object[i].value;
						holder[i].parent = object[i].parent;
					}
					holder[outsize-1].name = objname;
					holder[outsize-1].value = objvalue;
					holder[outsize].name = NULL;
					objname = NULL;
					objvalue = NULL;
					objnsize = 0;
					objvsize = 0;
					free(object);
					object = holder;//append the scanned values to output list
				}
				else {
					//if no entries have been recorded yet, just transfer ownership and reset placeholder variables
					object = new avf::Entry[2];
					object[0].name = objname;
					object[0].value = objvalue;
					object[1].name = NULL;
					objname = NULL;
					objvalue = NULL;
					objnsize = 0;
					objvsize = 0;
				}
				state = SI; // rinse and repeat
				break;
		}
	}
}

char* avf::get(FILE* target, char* key){
	return 0;
}
int avf::put(FILE* target, char* key, char* value){
	return 0;
}
//TODO: write these functions
