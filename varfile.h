#ifndef VARFILECONTROL_H
#define VARFILECONTROL_H
#include <stdio.h>

namespace avf{
	class Entry{
		public:
		char* name;
		enum{
			VALUE,
			STRING,
			OBJECT,
			ERROR
		}type;
		union{
			double value;
			char* string;
			Entry* children;
		};
		Entry* parent;
	};
	Entry* load(FILE* target);
	
	unsigned long write(FILE* target, Entry* values);

	unsigned long update(FILE* target, Entry* values);
}
#endif
