#ifndef VARFILECONTROL_H
#define VARFILECONTROL_H
#include <stdio.h>
#include <string>

class VarFile{
	char* fpath;
	void initarray(const void* ptr);
	public:
	VarFile(const char* fpathi);
	char* path();
	
	int get(const std::string& name, int* target);
	int get(const std::string& name, float* target);
	int get(const std::string& name, char** target);
	
	int set(const char* name, const int target);
	int set(const std::string& name, const float target);
	int set(const std::string& name, const char* target);
};

#endif
