#include "varFileControl.h"

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
int VarFile::get(const std::string name&, float* target){
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
int VarFile::get(const std::string name&, char** target){
	return 0;
};

int VarFile::set(const std::string name&, const int target){
	FILE* file = fopen(fpath, "r+");
	int c;
	std::string word;
	do{
		c=fgetc(file);
		if(c==(int)'\n' || c==EOF || c==' '){
			if(word==name){
				char buf=nullptr;
				word=="";
				size_t len;
				int tarh=target;
				for(len=0;tarh>0;len++)tarh=tarh/10;
				size_t inspos=ftell(file);
				fseek(file, 0, SEEK_END);
				fseek(file, inspos, SEEK_SET);
				while(c!=(int)'\n' || c!=EOF)c=fgetc(file);
				buf=new char[ftell(file)-inspos];
				fseek();
				fread(buf, );
				
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
int VarFile::set(const std::string name&, const float target){
	return 0;
};
int VarFile::set(const std::string name&, const char* target){
	return 0;
};
