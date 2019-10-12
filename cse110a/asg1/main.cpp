#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include "auxlib.h"

void scan_options(int argc, char** argv) { 
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            //cout << "Debug flags: " << optarg << endl;
            debug::setflags (optarg);
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
   return EXIT_SUCCESS;
}
