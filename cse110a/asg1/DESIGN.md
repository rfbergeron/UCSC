# Design Doc: `oc` String Set and Preprocessor
### Robert Bergeron
### ID: `rbergero`

## Details:
- Output files should have the .str extension
- Use `getopt(3)` to analyze options and arguments. Look at assignments from
    Advanced Programming (CS109) for example of how to use it.
- Need debug flags as well. Salvage from CS109 assignments.
    EDIT: use auxlib.h/.cpp from util-code/utility directory
- Use `basename(3)` to generate names of ouput files
- Use `fgets(3)` to read lines from the input files
- Use `strtok_r(3)` with spaces, tabs and newlines as delimiters, to tokenize the
    output of fgets
- Program reads the output of the program /usr/bin/cpp, not the raw file
- Use the interface providided in string_set.h/.cpp to store tokens once they
    have been scanned. Tokens will be stored in a hash table. The interface
    should be able to dump the string set and, of course, store new values.

## Required Components:
- getopt(3)
- basename(3)
- fgets(3)
- strtok_r(3)
- /usr/bin/cpp
- debug flags
- unordered_set

## Files:
- main.cpp
- auxlib.cpp
- auxlib.h
- string_set.cpp
- string_set.h

## Pseudocode (ugh):
```
const BUFFER_SIZE
int main(argc, argv) {
    buffer = new vector/malloc of size BUFFER_SIZE
    call getopt a bunch and send options to flex/bison/cpp
    fileindex = getopt()
    
    for(FILE from fileindex to argc) {
        status = fgets(buffer, BUFFER_SIZE, FILE)
        while(status is not null) {
            while(tok = strtok(buffer, DELIMS) != null) {
                hash tok
                query hash in map
                if hash doesn't already have a value in
                    the map, put tok in it
            } 
        }
    }
} 
```
