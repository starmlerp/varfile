#ifndef VARFILECONTROL_H
#define VARFILECONTROL_H
#include <stdio.h>

namespace avf{
	class Entry{
		public:
		char* name;
		char* value;
		Entry* parent;
	};
	Entry* load(FILE* target);

	unsigned long write(FILE* target, Entry* values);

	char* get(FILE* target, char* key);
	
	int put(FILE* target, char* key, char* value);
	
}
#endif
