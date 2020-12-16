#include "varfile.h"

VarFile::VarFile(const char* fpathi){
	fpath=(char*)fpathi;
	FILE* f=fopen(fpath, "r");
	if(f==NULL)f=fopen(fpath, "w");
	fclose(f);
}
char* VarFile::path(){
	return fpath;
}
/*
void initarray(void* ptr, size_t size){
	if(ptr!=NULL)free(ptr);
	ptr=malloc(size+1);
	ptr[size]=NULL;
}
*/
int VarFile::get(const std::string& name, int* target){
	FILE* file = fopen(fpath, "r");
	int c;
	std::string word;
	do{
		c=fgetc(file);
		if(c==(int)'\n' || c==EOF || c==' '){
			if(word==name){
				word="";
				char input;
				while(fread(&input, 1, 1, file) && input != ' ' && input != '\n')word+=input;
				*target=std::stoi(word);
				fclose(file);
				return 0;
			}
			word="";
		}
		else word+=(char)c;
		
	}while(c!=EOF);
	fclose(file);
	return 1;
}
int VarFile::get(const std::string& name, float* target){
	FILE* file = fopen(fpath, "r");
	int c;
	std::string word;
	do{
		c=fgetc(file);
		if(c==(int)'\n' || c==EOF || c==' '){
			if(word==name){
				word="";
				char input;
				while(fread(&input, 1, 1, file) && input != ' ' && input != '\n')word+=input;
				*target=std::stof(word);
				fclose(file);
				return 0;
			}
			word="";
		}
		else word+=(char)c;
		
	}while(c!=EOF);
	fclose(file);
	return 1;
};
int VarFile::get(const std::string& name, char** target){
	return 0;
};

int VarFile::set(const char* name, const int target){
	FILE* file = fopen(fpath, "r+");
	int c;
	size_t wsize=0;
	size_t tsize;
	for(tsize=0;target[tsize];tsize++);
	char* word;
	do{
		c=fgetc(file);//read a single character
		if((c==(int)'\n' || c==EOF || c==(int)' ')){//if its newline, end of file, of spacing
			if(wsize==tsize){
				int cmpwrd=1;
				for(int i=0; cmpwrd && i<wsize; i++)cmpwrd=word[i]==target[i];//compare the 2 strings
				if(cmpwrd){
					free(word);
					size_t ins=ftell(file);
					do{c=fgetc(file);}while(c!=(int)'\n' || c!=EOF || c!=(int)' ');//find the point at which targeted entry ends
					size_t hold=ftell(file);
					fseek(file, 0L, SEEK_END);
					char* buf=(char*)malloc[ftell(file)-hold];//set the buffer size to the length after the data pointed to until the end of file
					fseek(file, hold, SEEK_SET);
					fread(buf, sizeof(buf), 1, file);//store the entries coming after the targeted entry
					fseek(file, 0L, ins);
					fprintf(file, "%d\n%s", target, buf);
				}
			}
			free(word);
		}
		else{ 
			char* holder=NULL;
			initarray(holder, wsize+1);
			for(int i=0; i<wsize; i++)holder[i]=word[i];//copy word into holder
			holder[++wsize]=(char)c;//add the read character
			free(word);//deallocate word
			word=holder;//put the location of holder array into word
		}

	}while(c!=EOF);
	char* out;
	fseek(file, 0, SEEK_END);
	fprintf(file, "%s %d\n",name, target);
	fclose(file);
	return 0;
};
int VarFile::set(const std::string& name, const float target){
	return 0;
};
int VarFile::set(const std::string& name, const char* target){
	return 0;
};
