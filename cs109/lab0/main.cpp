// $Id: main.cpp,v 1.5 2018-06-19 17:36:22-07 - - $

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <libgen.h>

#include "hello.h"

int main (int argc, char** argv) {
   string execname {basename (argv[0])};
   vector<string> args (&argv[1], &argv[argc]);
   cout << "Execname: " << execname << ", Compiled: "
        << __DATE__ << " " << __TIME__ << endl;
   cout << "This is: " << __FILE__ << "[" << __LINE__ << "]: "
        << __PRETTY_FUNCTION__ << endl;
   auto hello_ptr = make_shared<hello>();
   hello_ptr->say (cout);
   hello goodbye {"Goodbye, world!"};
   goodbye.say (cout);
   hello second {*hello_ptr};
   second.say (cout);
   for (const auto& arg: args) cout << "for: " << arg << endl;
   for_each (args.cbegin(), args.cend(),
             [](auto& arg) { cout << "for_each: " << arg << endl; });
   return EXIT_SUCCESS;
}

