#ifndef VARFILECONTROL_H
#define VARFILECONTROL_H
#include <stdio.h>

namespace avf{
	class Entry{
		public:
		char* name;
		char* value;
		void* parent;
	};
	Entry* load(FILE* target);

	char* getEntries(FILE* target);

	char* getEntries(FILE* target, char* object);

	char* get(FILE* target, char* key);
	
	int put(FILE* target, char* key, char* value);
	
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
