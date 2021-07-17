#ifndef VARFILECONTROL_H
#define VARFILECONTROL_H
#include <stdio.h>

namespace avf{
	class Index{
		public:
		char* word;
		unsigned long* pos;
	};
	Index* index(FILE* target, short wlen);
	
	char* get(FILE* target, char* key);
	char* get(FILE* target, char* key, Index* index);
	
	int put(FILE* target, char* key, char* value);
	int put(FILE* target, char* key, char* value, Index* index);
	
}
/*
class VarFile{
	char* fpath=NULL;
	public:
	void path(const char* _fpath);
	VarFile(const char* _fpath);
	char* path();
	int load(const adn::datanode* tree);
};
*/
#endif
