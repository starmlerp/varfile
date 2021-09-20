# About
TODO: this manual is invalid now. rewrite it
Varfile is a static C++ library, made with intention of abstracting
file loading functionality in game engines.

The syntax format is somewhat controllable my manipulating preprocessor calls at
the head of the source code.

# Usage
To use the library, first include the header file in your code:

``` c++
#include "varfile/vafile.h"

int main(void){
	//...
	return 0;
}
```
and link it during compiling:
`g++ -I path/to/libraries -lvarfile soure.cpp`
## documentation
### classes
```c++
class Entry{
	public:
	char* name;
	char* value;
	Entry* parent;
};
``` 
Library functions use this class as a container for extracting data from
inputted files as well as writing data to it

### functions
```c++
Entry* load(FILE* target);
``` 
Returns a list of data from filestream `target`. For each entry, class element
`Entry::word` is a null-terminated `char` array, class element `Entry::value`
is a `char` array, with each array entry a literal string found at the position
of the cursor.

library assumes stream is in read mode.

```c++
unsigned long write(FILE* target, Entry* values);
``` 

Writes to the stream from the inputted `Entry*` array.

## contributing

Feel free to open an issue. If you want to add a feature, firstly open an issue
to discuss it, then make a pull request.

If you have any questions, feel free to ask them in the form of an issue, but
firstly look if it was already answered.
