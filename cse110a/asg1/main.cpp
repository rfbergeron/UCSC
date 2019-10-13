#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include "auxlib.h"

void scan_options(int argc, char** argv) { 
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:D:ly");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debug::setflags (optarg);
            break;
         case 'D':
            // cpp args
            DEBUGF('s', "cpp option: " << optarg);
            break;
         case 'l':
            DEBUGF('s', "yy_flex_debug set to 1");
            break;
         case 'y':
            DEBUGF('s', "yydebug set to 1");
            break;
         default:
            cerr << "-" << char (optopt) << ": invalid option"
               << endl;
            break;
      }
   }
}

int main(int argc, char** argv) {
   scan_options(argc, argv);
   // calls to getopt increment optind, so after scan_options returns
   // we should be at the name of the target file.

   return EXIT_SUCCESS;
}
