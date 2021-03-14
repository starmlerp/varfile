#ifndef _DATANODE_H_
#pragma error This library depends on the datanode library. You must include it before this one.
#endif
#ifndef VARFILECONTROL_H
#define VARFILECONTROL_H
#include <stdio.h>

class VarFile{
	char* fpath=NULL;
	public:
	void path(const char* _fpath);
	VarFile(const char* _fpath);
	char* path();
	int load(const datanode* tree);
};

#endif
