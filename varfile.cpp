#include "varfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <stack>

#define FRIVOLOUS " \n\t" //list of symbols parser should ingore entirely

#define CORRELATOR '='
#define TERMINATOR ';'
#define DECIMAL '.'
#define QUOTE '\"'
#define BRACKETS "{}"

#define FPRECISION 10

//#define DEBUG

int isFrivolous(char inT){//this is a private function, neccesary only for the function of this library, as such it is not neccesary to make it visible to the main code
	for(size_t i=0; FRIVOLOUS[i]; i++)if(inT == FRIVOLOUS[i])return 1;
	return 0;
}

avf::Entry* avf::load(FILE* target){
	rewind(target);
	size_t outsize = 0;
	char inC;
	char quotes=0;
	do{
		inC = fgetc(target);
		if(!quotes && (inC == TERMINATOR || inC == BRACKETS[0]))outsize++; // count up all of the entries
		if(inC == QUOTE)quotes = 1 - quotes;
		if(inC == '\\')fgetc(target);
	}while(inC != EOF);// runs through the whole file
	#ifdef DEBUG
	printf("counted entries: %ld\n", outsize);
	#endif
	rewind(target);
	inC = 0;
	size_t outpos = 0;
	avf::Entry* object = new avf::Entry[outsize+1];
	object[0].name = NULL;
	object[outsize].name = NULL;
	object[outsize].type = avf::Entry::EOE;
	//TODO: implement proper parsing to detect invalid variable names (starting with numbers, including non-aphlanumerical characters etc.)
	
	enum {// we shall represent states using enums and switch-cases
		EE, // end of entry
		SI, // seek for the next identifier
		ID, // identifier
		SC, // seek for the correlator
		CR, // correlator
		SV, // seek for the value
		OV, // value
		OF, //decimal part
		QU, // quote
		QE, // escaped character in quotes
		BL, // block (defines multi member objects)
		CM  // comment
	}state = SI;

	long line=1;
	// element holder values
	size_t objnsize=0, 
	       objssize=0, 
	       objvfsize=1;
	char* objname;
	char* objstring;
	double objvalue;
	std::stack<avf::Entry*> objects;
	std::stack<size_t> ocsize;
	inC = fgetc(target);
	while(true){
		// we are entering an infinite loop, since the following code is going to be implemented as a state machine
		// the loop is going to be broken on a return call
		#ifdef DEBUG
		printf("input: %c(%d), state: %d\n", inC=='\n'?'N':inC, (int)inC, state);
		fflush(stdout);
		#endif
		if(inC=='\n')line++;//keep track of lines, will be used to display errors
		//TODO: above line is prone to bugs. have line counter increment with each state, not like this
		if(inC==EOF){
			if(state==SI && objects.empty()){
				return object;
			}
			else if(state != SI){
				delete [] object;
				object = new avf::Entry[1];
				object[0].name = new char[1];
				object[0].name[0] = 0;
				object[0].type = avf::Entry::ERROR;
				object[0].string = new char[200];
				sprintf(object[0].string, "line %ld: unexpected end-of-file\n\0", line);
				return object;
			}
			else if(!objects.empty()){
				delete [] object;
				object = new avf::Entry[1];
				object[0].name = new char[1];
				object[0].name[0] = 0;
				object[0].type = avf::Entry::ERROR;
				object[0].string = new char[200];
				sprintf(object[0].string, "line %ld: missing %ld enclosing brackets\n\0", line, objects.size());
				return object;
			}
		}
		switch(state){
			case SI:
				if(isFrivolous(inC))inC = fgetc(target);
				else if(inC == BRACKETS[1]){ // if brace closing is found
					objects.pop();//pop the stack
					ocsize.pop();
					inC = fgetc(target);
				}
				else if(inC >= 'a' && inC <= 'z' || inC >= 'A' && inC <= 'Z'){
					state = ID;
				}// otherwise assume we reached an identifier
				else {
					delete [] object;
					object = new avf::Entry[1];
					object[0].name = new char[1];
					object[0].name[0] = 0;
					object[0].type = avf::Entry::ERROR;
					object[0].string = new char[200];
					sprintf(object[0].string, "line %ld: identifier cannot start with \"%c\"\n\0", line, inC);
					return object;
				}
				break;
			
			case ID:
				if(objnsize==2){
					//TODO: following condition is impossible to meet. fix that
					if(objname[0] == '/' && objname[1] == '/'){// if the input appears to be a start of comment
						objnsize=0;
						delete [] objname;
						objname=NULL;
						state = CM;// mark it as such
						inC=fgetc(target);
						break;
					}
				}
				if(inC >= 'A' && inC <= 'Z' || inC >= 'a' && inC <= 'z' || inC >= '0' && inC <= '9'){// if its an alphanumerical character
					char* holder = new char[++objnsize+1];// add it to the checking stream
					//holder is an additional character long in order to hold null terminator
					for(size_t i = 0; i < objnsize-1;i++)holder[i]=objname[i];
					holder[objnsize-1]=inC;
					if(objnsize > 1)delete [] objname;
					objname=holder;//dirty dirty append
					inC = fgetc(target);
					break;
				}

				else if(isFrivolous(inC)){//end of identifier
					objname[objnsize]='\0';
					state=CR;//assume we are looking for a correlator
					break;
				}

				else {
					delete [] object;
					object = new avf::Entry[1];
					object[0].name = new char[1];
					object[0].name[0] = 0;
					object[0].type = avf::Entry::ERROR;
					object[0].string = new char[200];
					sprintf(object[0].string, "line %ld: invalid character (\"%c\") in variable declaration\n\0", line, inC);
					return object;
				}
			case CR:
				if(inC == CORRELATOR){
					state = SV;
					inC = fgetc(target);
				}
				
				else if(inC == BRACKETS[0]){
					outpos++; // record to output array
					object[outpos-1].name = objname;
					object[outpos-1].type = avf::Entry::OBJECT;
					object[outpos-1].value = 0.0;
					object[outpos].name = NULL;
					objname = NULL;
					objnsize = 0;
					
					if(!objects.empty())object[outpos-1].parent=objects.top(); // if theres something in the stack, put it in as a parent
					else object[outpos-1].parent = NULL;
					
					objects.push(&object[outpos-1]);
					
					size_t esize=0;
					long objpos = ftell(target);
					int brackets = 0;
					size_t blayers=0;

					while(blayers || brackets ||  inC != BRACKETS[1]){//load the children elements into array
						inC = fgetc(target);
						#ifdef DEBUG
						printf("%ldinput: %c(%d), substate: %d\n", blayers, inC=='\n'?'N':inC, (int)inC, brackets);
						fflush(stdout);
						#endif
					
						if(inC == QUOTE)brackets = 1 - brackets;
						else if(inC == TERMINATOR && !blayers && !brackets)esize++;
						else if(inC == BRACKETS[0])blayers++;
						else if(inC == BRACKETS[1])if(blayers)blayers--;
						else if(inC == EOF){
							delete [] object;
							object = new avf::Entry[1];
							object[0].name = new char[1];
							object[0].name[0]=0;
							object[0].type = avf::Entry::ERROR;
							object[0].string = new char[200];
							sprintf(object[0].string, "line %ld: missing enclosing brackets\n\0", line);
							return object;
						}
						else if(inC == '\\')fgetc(target);
					}
					#ifdef DEBUG
					printf("entries in this layer: %ld\n", esize);
					#endif
					objects.top()->children = new avf::Entry*[esize+1];
					for(size_t i = 0; i < esize; i++)objects.top()->children[i] = NULL;
					objects.top()->children[esize] = new avf::Entry;
					objects.top()->children[esize]->type = avf::Entry::EOE;
					
					fseek(target, objpos, SEEK_SET);
					ocsize.push(esize);
					
					state = SI; // rinse and repeat
				}
				else if(isFrivolous(inC));
				else {
					delete [] object;
					object = new avf::Entry[1];
					object[0].name = new char[1];
					object[0].name[0]=0;
					object[0].type = avf::Entry::ERROR;
					object[0].string = new char[200];
					sprintf(object[0].string, "line %ld: expected \"%c\", got \"%c\"\n\0", line, CORRELATOR, inC);
					return object;
				}
				inC = fgetc(target);
				break;

			case SV:
				if(isFrivolous(inC)); // ignore anything that could not be an object
				else if(inC == QUOTE){
					state = QU;
					object[outpos].type = avf::Entry::STRING;
					objstring = NULL;
				}
				else if(inC == '-'){
					state = OV;
					object[outpos].type = avf::Entry::VALUE;
					objvfsize = -1;
				}
				else {
					state = OV;
					object[outpos].type = avf::Entry::VALUE;
					objvalue = 0.0;
					break;
				}
				inC = fgetc(target);
				break;
			case OV:
				if(inC == TERMINATOR){
					objvalue *= objvfsize;
					objvfsize = 1;
					state = EE;
				}
				else if(inC == DECIMAL){
					state = OF;
				}
				else if(inC >= '0' && inC <= '9'){ // if our value is numeric
					objvalue = objvalue * 10 + (inC - '0');
				}
				else if(isFrivolous(inC));
				else{
					free(object);
					object = new avf::Entry[1];
					object[0].name = new char[1];
					object[0].name[0] = 0;
					object[0].type = avf::Entry::ERROR;
					object[0].string = new char[200];
					sprintf(object[0].string, "line %ld: unexpected value for %s: %c\n\0", line, objname, inC);
					return object;
				}
				inC = fgetc(target);
				break;
			case OF:
				if(inC >= '0' && inC <= '9'){
					objvfsize*=10;
					objvalue = objvalue * 10 + (inC -'0');
				}
				else if(isFrivolous(inC));
				else if(inC == TERMINATOR){
					state = EE;
					objvalue /= objvfsize;
					objvfsize = 1;
				}
				else{
					free(object);
					object = new avf::Entry[1];
					object[0].name = new char[1];
					object[0].name[0] = 0;
					object[0].type = avf::Entry::ERROR;
					object[0].string = new char[200];
					sprintf(object[0].string, "line %ld: unexpected value for %s: %c\n\0", line, objname, inC);
					return object;
				}
				inC = fgetc(target);
				break;

			case QU:
				if(inC == QUOTE)state = OV; // switch to end state: we know nothing more is here
				else if(inC == '\\')state = QE; // switch to escape state
				else {
					char* holder = new char[++objssize+1]; // apppend it
					for(size_t i = 0; i < objssize-1;i++)holder[i]=objstring[i];
					holder[objssize-1]=inC;
					if(objssize > 1)delete [] objstring;
					objstring=holder;
				}
				inC = fgetc(target);
				break;
			case QE:{
				char* holder = new char[++objssize+1]; // apppend it
				for(size_t i = 0; i < objssize-1;i++)holder[i]=objstring[i];
				holder[objssize-1]=inC;
				if(objssize > 1)delete [] objstring;
				objstring=holder;
				state = QU;
				inC = fgetc(target);
				break;
			}
			case EE:
				outpos++;
				#ifdef DEBUG
				if(object[outpos-1].type == avf::Entry::STRING)printf("creating entry %ld: \"%s\" = \"%s\"\n", outpos, objname, objstring);
				if(object[outpos-1].type == avf::Entry::VALUE)printf("creating entry %ld: \"%s\" = %.2f\n", outpos, objname, objvalue);
				#endif
				object[outpos-1].name = objname;
				if(object[outpos-1].type == avf::Entry::VALUE) object[outpos-1].value = objvalue;
				else if(object[outpos-1].type == avf::Entry::STRING) object[outpos-1].string = objstring;
				if(!objects.empty()){
					object[outpos-1].parent = objects.top();
					size_t cpos;
					for(cpos = 0; objects.top()->children[cpos]; cpos++){
						if(!objects.top()->children[cpos]){
							if(objects.top()->children[cpos]->type == avf::Entry::EOE){
								//this should really not happen, meaning we fucked something up big time
								delete [] object;
								object = new avf::Entry[1];
								object[0].name = new char[1];
								object[0].name[0] = 0;
								object[0].type = avf::Entry::ERROR;
								object[0].string = new char[200];
								sprintf(object[0].string, "internal error: parent has no more memory to store pointer to child\n\0", line, objname, inC);
								return object;
							}
						}
					}
					objects.top()->children[cpos] = &object[outpos-1];
				}
				else object[outpos-1].parent = NULL;
				objname = NULL;
				objstring = NULL;
				objvalue = 0.0;
				objnsize = 0;
				objssize = 0;
				state = SI; // rinse and repeat
				break;
		}
	}
}

size_t avf::write(FILE* target, avf::Entry* values){
	size_t outlen = 0; // tracks the number of characters written

	size_t Elen;
	for(Elen = 0; values[Elen].type != avf::Entry::EOE; Elen++);
	#ifdef DEBUG
	printf("%ld\n", Elen);
	#endif
	avf::Entry** holder = new avf::Entry*[Elen+1];//create references to the input array. they will be removed as they are being written
	for(size_t i = 0; i < Elen; i++)holder[i] = &values[i];
	
	std::stack<avf::Entry*> layers;
	size_t i = 0;
	while(i < Elen || !layers.empty()){
		#ifdef DEBUG
		printf("elem: %ld\n", i);
		#endif
		if(holder[i]){
			#ifdef DEBUG
			//printf("processing element %ld\n", i);
			//if(!layers.empty())printf("stack: %ld\n", layers.top());
			#endif
			if(holder[i]->type == avf::Entry::EOE){
				printf("EOE\n");
			}
			else if(holder[i]->type != avf::Entry::OBJECT){//if its a regular entry
					#ifdef DEBUG
					printf("element %d is a value\n", i);
					#endif
					char* tabs = NULL;
					if(!holder[i]->parent && layers.empty()){
						tabs = new char[1];
						tabs[0]='\0';
					}
					else if(holder[i]->parent == layers.top()){
						tabs = new char[layers.size()+1];
						for(size_t j = 0; j < layers.size(); j++)tabs[j]='\t';
						tabs[layers.size()] = '\0';
					}
					if(holder[i]->type==avf::Entry::VALUE && tabs){
						outlen += fprintf(target, "%s%s %c %ld", tabs, holder[i]->name, CORRELATOR, (long)holder[i]->value);
						delete [] tabs;
						double fp = holder[i]->value - (long)holder[i]->value;
						if(fp!=0)fprintf(target, "%c", DECIMAL);
						int dp=0;
						char outf[FPRECISION];
						short outfs=0;
						outf[0]='\0';
						while(fp != 0 && dp < FPRECISION){
							fp *= 10;
							outf[outfs]=(char)((long)fp+'0');
							outf[++outfs]='\0';
							if( (long)fp > 0){
								fprintf(target, "%s", outf);
								outfs=0;
								outf[0]='\0';
							}
							fp -= (long)fp;
							dp++;
						}
						outlen += fprintf(target, "%c\n", TERMINATOR);
						holder[i] = NULL;
					}
					else if(holder[i]->type==avf::Entry::STRING && tabs){
						outlen += fprintf(target, "%s%s %c \"%s\"%c\n", tabs, holder[i]->name, CORRELATOR, holder[i]->string, TERMINATOR);
						delete [] tabs;
						holder[i] = NULL;
					}
					i++;
			}
			else{//if its a nest
				if(!holder[i]->parent && layers.empty()){
					outlen += fprintf(target, "%s %c\n", holder[i]->name, BRACKETS[0]);
					#ifdef DEBUG
					printf("object definition: %ld\n", holder[i]);
					#endif
					layers.push(holder[i]);
					holder[i]=NULL;
					i = 0;
				}
				else if(holder[i]->parent == layers.top()){
					char* tabs = new char[layers.size()+1];
					for(size_t j = 0; j < layers.size(); j++)tabs[j]='\t';
					tabs[layers.size()]='\0';
					outlen += fprintf(target, "%s%s %c\n", tabs, holder[i]->name, BRACKETS[0]);
					#ifdef DEBUG
					printf("object definition: %ld\n", holder[i]);
					#endif
					layers.push(holder[i]);
					holder[i]=NULL;
					i = 0;
				}
				else i++;
			}
		}
		else {
			i++;
			#ifdef DEBUG
			printf("already processed. skipping...\n");
			#endif
		}
		if( i >= Elen && !layers.empty()){
			#ifdef DEBUG
			printf("loaded all objects in current layer. popping stack...\n");
			#endif
			layers.pop();
			char* tabs = new char[layers.size()+1];
			for(size_t j = 0; j < layers.size(); j++)tabs[j]='\t';
			tabs[layers.size()]='\0';
			outlen += fprintf(target, "%s%c\n", tabs, BRACKETS[1]);
			delete [] tabs;
			i=0;
		}
		#ifdef DEBUG
		fflush(target);
		#endif
	}
	return outlen;
}

avf::Entry* avf::get(avf::Entry* values, char* name){
	for(size_t i = 0; values[i].type != avf::Entry::EOE; i++){
		int valid = 1;
		for(size_t j = 0; values[i].name[j] && name[j]; j++){
			if(values[i].name[j] != name[j]){
				valid = 0;
				break;
			}
		}
		if(valid)return &values[i];
	}
	return NULL;
}

unsigned long avf::update(FILE* target, avf::Entry* values){
	return 0;
}
