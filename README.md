# Varfile 
Varfile is a static C++ library, made with intention of abstracting
and speeding up file loading in game engines
# Usage
To use the library, first include the header file in your code:
> main.cpp
``` c++
#include "varfile/vafile.h"

int main(void){
	//...
	return 0;
}
```
and link it during compiling:
`g++ -I path/to/libraries -lvarfile main.cpp`
## documentation
### classes
```c++
class Index{
	public:
	char* word;
	unsigned long* pos;
};
``` 
Library functions use this class to create file indices and increase performance during
parsing of larger files
### functions
```c++
Index* index(FILE* target, short wlen);
``` 
Creates an index of file `target` with the precision of `wlen`. For each
index entry, class element `Index.word` is a null-terminated `char` arrayof
length `wlen + 1`, class element `Index.pos` is an `unsigned long` array, with
each array entry representing cursor position in the file stream where an
expression with then corrensponing `Index.word` was found to be the part at the
beginning of it. outputted `Index*` object can be used by other commands to aid
in parsing of the filestream. 

It is reccomended to recreate index each time the program which the library is used
in is started, in case the file has been manually edited by the user.

## contributing

Feel free to open an issue. If you want to add a feature, firstly open an issue to discuss it, then make a pull request.

If you have any questions, feel free to ask them in the form of an issue, but firstly look if it was already answered.
