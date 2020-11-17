#include "varfile.h"

VarFile::VarFile(const char* fpathi){
	fpath=(char*)fpathi;
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

int VarFile::set(const std::string& name, const int target){
	FILE* file = fopen(fpath, "r+");
	int c;
	std::string word;
	do{
		c=fgetc(file);
		if(c==(int)'\n' || c==EOF || c==(int)' '){
			if(word==name){
				word="";
				size_t ins=ftell(file);
				do{c=fgetc(file);}while(c!=(int)'\n' || c!=EOF || c!=(int)' ');//find the point at which targeted entry ends
				size_t hold=ftell(file);
				fseek(file, 0L, SEEK_END);
				char* buf=new char[ftell(file)-hold];//set the buffer size to the length after the data pointed to until the end of file
				fseek(file, hold, SEEK_SET);
				fread(buf, sizeof(buf), 1, file);//store the entries coming after the targeted entry
				fseek(file, 0L, ins);
				fprintf(file, "%d\n%s", target, buf);
			}
			word="";
		}
		else word+=(char)c;

	}while(c!=EOF);
	char* out;
	fseek(file, 0, SEEK_END);
	sprintf(out, "%s %d\n",name.c_str(), target);
	fwrite(out, sizeof(out), 1, file);
	fclose(file);
	return 0;
};
int VarFile::set(const std::string& name, const float target){
	return 0;
};
int VarFile::set(const std::string& name, const char* target){
	return 0;
};
