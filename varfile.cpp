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
	for(tsize=0;name[tsize];tsize++);
	char* word;
	do{
		c=fgetc(file);//read a single character
		if((c==(int)'\n' || c==EOF || c==(int)' ')){//if its newline, end of file, of spacing
			if(wsize==tsize){
				int cmpwrd=1;
				for(int i=0; cmpwrd && i<wsize; i++)cmpwrd=word[i]==name[i];//compare the 2 strings
				if(cmpwrd){
					if(wsize)free(word);
					size_t ins=ftell(file);
					do{c=fgetc(file);}while(c!=(int)'\n' && c!=EOF && c!=(int)' ');//find the point at which targeted entry ends
					size_t hold=ftell(file);
					fseek(file, 0L, SEEK_END);
					size_t fsize = ftell(file);
					char* buf=(char*)malloc(fsize-hold);//set the buffer size to the length after the data pointed to until the end of file
					fseek(file, hold, SEEK_SET);
					fread(buf, sizeof(buf), 1, file);//store the entries coming after the targeted entry
					fseek(file, 0L, ins);
					fprintf(file, "%d\n%s", target, buf);
					fclose(file);
					return 0;
				}
			}
			if(wsize)free(word);
			wsize=0;
		}
		else{
			char* holder=(char*)malloc(wsize+1);
			
			for(int i=0; i<wsize; i++)holder[i]=word[i];//copy word into holder
			holder[wsize++]=(char)c;//add the read character
			if(wsize-1>0)free(word);//deallocate word
			word=holder;//put the location of holder array into word
		}

	}while(c!=EOF);
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
