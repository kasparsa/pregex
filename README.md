<p align="center">
  <img src="./logo.png" />
</p>

# pregex
A lightweight and fast C/C++ library for regex parsing and tokenizing results, initially designed as a pure ANSI C project for the AVR micro-controllers later converted to a C++ class.

To compile for a Release build (performance-optimized, default):
```
mkdir build
cd build
cmake ..
make
```
To configure for a Debug build (with debugging flags):
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```
Usage to write regular expressions with colorized pattern matching and debugging :
```
./parser ../testdata/testdata01.in "\n\([A-Z]+\);\([A-Za-z ()/0-9]+\);\([0-9]+\)"
```
## Usage from your C/C++ program
Compile as release edition and install library with header file:
```
mkdir build
cd build
cmake ..
make
sudo make install
```
Write as simple C/C++ test program:
```
#include <pregex/pregex.h>

int main( int argc, char *argv[] )
{
  PRegEx RegEx;
  
  RegEx.init();
  
  RegEx.verbosity(0);
  
  RegEx.load( "../testdata/testdata01.in" );
  
  RegEx.regextract((char*) "\\n\\([A-Z]+\\);\\([A-Za-z ()/0-9]+\\);\\([0-9]+\\)" );

  unsigned long count = RegEx.getrowcount();

  return 0;
}
```
