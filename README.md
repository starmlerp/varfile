# About
Varfile is a static C++ library, made with intention of abstracting
file loading functionality.
This library has been written with explicit purpose of being used in one
individuals game engine, and provides no guarantee of fitness for particular
use case.

The syntax format is somewhat controllable my manipulating preprocessor calls at
the head of the source code.

# Usage
To use the library, first compile it into a binary archive:
`$ g++ -c [FLAGS] varfile.cpp -o varfile.o`
`$ ar rcs libvarfile.a varfile.o`

Then include the header file in your code:
``` c++
#include "varfile/vafile.h"

int main(void){
	//...
	return 0;
}
```
and link it during compiling:
`$ g++ -I path/to/libraries -lvarfile soure.cpp`
## documentation
### classes
```c++
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
	};
	Entry* parent;
};
``` 
Library functions use this class as a container for extracting data from
inputted files as well as writing data to it

### functions
```c++
Entry* load(FILE* target);
``` 
Returns a list of data from filestream `target`. For each object, class element
`avf::Entry::name` is a null-terminated `char` array, representing the identifier
the entry was found with. the enum `avf::Entry:type` determines the type of the
value the object holds:
if it is `avf::Entry::VALUE`, then `avf::Entry::value` holds a `double` value,
if it is `avf::Entry::STRING`, then `avf::Entry::string` holds a `char*` array,
if it is `avf::Entry::OBJECT`, then object represents a nested object,
if it is `avf::Entry::ERROR`, then the returned pointer should point to a singular object, with `avf::Entry::string` containing the error message

library assumes stream is in read mode.

```c++
unsigned long write(FILE* target, Entry* values);
``` 

Writes to the stream from the inputted `Entry*` array, and returns the number
of written objects.

this function assumes none of the objects have their `avf::Entry::type` set to "avf::Entry::ERROR"

## contributing

Feel free to open an issue. If you want to add a feature, firstly open an issue
to discuss it, then make a pull request.

If you have any questions, feel free to ask them in the form of an issue, but
firstly look if it was already answered.
