<p align="center">
  <img src="./logo.png" />
</p>

# PRegEx
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
Write as simple C/C++ test program test.cpp:
```
#include <pregex/pregex.h>

int main( int argc, char *argv[] )
{
  PRegEx RegEx;
  
  RegEx.init();
  
  RegEx.verbosity(2);
  
  RegEx.load( "../testdata/testdata01.in" );
  
  RegEx.regextract((char*) "\\n\\([A-Z]+\\);\\([A-Za-z ()/0-9]+\\);\\([0-9]+\\)" );

  int row_count = RegEx.getrowcount();
  int col_count = RegEx.getcolcount();

  for ( int r = 0; r < row_count; r++ ) {
        for ( int c = 0; c < col_count; c++ ) {
                printf( "%s ", RegEx.getval( r, c ) );
        }
        printf("\n");
  }

  return 0;
}

```
Compile link and run:
```
g++ test.cpp -o test -lPRegEx
./test
```

## Overview
PRegEx is a lightweight C++ library for regular expression (regex) pattern matching and result tokenization. It is designed to provide efficient matching of complex patterns and tokenizing regex groups into structured results. PRegEx offers support for custom regex patterns, handling special characters, tokenization, and result extraction.

## Key Features:
- **Custom Regex Matching:** Supports matching against a wide variety of patterns.
- **Result Tokenization:** Groups matching tokens and provides structured access to matched data.
- **Balanced Symbol Detection:** Handles balanced symbols like parentheses, brackets, and braces.
- **File and Buffer Support:** Can load regex patterns from files or in-memory buffers.
- **Configurable Verbosity:** Provides detailed debug output based on verbosity level.

# API Reference
**Class:** ```PRegEx```
### Public Methods:
- ```PRegEx():``` Constructor, initializes the regex engine.
- ```~PRegEx():``` Destructor, cleans up memory.
- ```void verbosity(int level):``` Sets verbosity level for debug information. Higher levels give more detailed output.
- ```void init():``` Initializes internal structures, preparing the engine for pattern processing.
- ```void load(const char* filename):``` Loads input data from a file for regex processing.
- ```void load(const char* str, unsigned int len):``` Loads input data from a buffer.
- ```void regextract(char* regexpat):``` Extracts matching patterns from the loaded data using the provided regex pattern.
- ```void deinit():``` Deinitializes the regex engine, freeing all allocated resources.
- ```unsigned long getrowcount():``` Returns the number of matching rows.
- ```unsigned long getcolcount():``` Returns the number of matching columns.
- ```char* getval(unsigned long row, unsigned long col):``` Retrieves the matched value at the specified row and column.
- ```unsigned long getval_start(unsigned long row, unsigned long col):``` Gets the start position of a matched value.
- ```unsigned long getval_end(unsigned long row, unsigned long col):``` Gets the end position of a matched value.

### Pattern Processing Methods:

- ```void tokenize_groups(char *buf, unsigned long fs, const unsigned long *src, tmatch *match, unsigned long col_cnt):``` Tokenizes regex groups into structured results.
- ```unsigned long process_pattern(char *pattern, tpat *pat):``` Processes a regex pattern into an internal format.
- ```void clear_pattern(tpat *pat, unsigned long cnt):``` Clears a pattern.
- ```void release_pattern(tpat *pat, unsigned long cnt):``` Releases memory used by a pattern.

### Match Debugging:

- ```void print_tokenized_groups(tmatch *match, unsigned long col_cnt):``` Prints tokenized match groups.
- ```void print_match_debug(char *buf, unsigned long len, unsigned long *src):``` Prints the matched pattern along with source data.

### Other Utility Methods:

- ```char* replace_substr(char* str, char* substr, unsigned long pos, unsigned long len):``` Replaces a substring within a string.
- ```unsigned long subset_match(char c, char* set):``` Checks if a character is part of a subset.
- ```unsigned long substring_match(unsigned long pos, const char *buf, char* str):``` Matches a substring in a buffer.
