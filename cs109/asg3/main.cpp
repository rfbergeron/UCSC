// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <regex>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

int main (int argc, char** argv) {
   sys_info::execname (argv[0]);

   scan_options (argc, argv);

   // Before processing, each input line echo out to cout
   // preceeded by filename, line # from file
   // Name of cin printed with '-' sign

   regex comment_regex {R"(^\s*(#.*)?$)"};
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};

   ifstream infile("test.txt");
   string line;
   smatch result;
  while (getline(infile, line)) {
    if(infile.peek()!=EOF){
      if(regex_search (line, result, comment_regex )){
         // search found comment, print line to output
         cout << "comment regex found, print out line.." << endl;
         cout << "line: " << line << endl;
      }
      if(regex_search (line, result, key_value_regex)){
         cout << "comment regex found, print out key: val: ..." << endl;
         // Need to call insert function with
         // key: result[1]
         // value: result[2]
         cout << "line: " << line << endl;
         cout << "key = " << result[1] << endl;
         cout << "value = " << result[2] << endl;
      } else if(regex_search (line, result, trimmed_regex)){
         cout << "trimmer regex found..." << endl;
         cout << "line: " << line << endl;
            // Only a = sign with key
            cout << "key = " << result[1] << endl;
            // Only a = sign with value
            cout << "value = " << result[2] << endl;
      }
    }
  }

   str_str_map test;
   for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
      str_str_pair pair (*argp, to_string<int> (argp - argv));
      cout << "Before insert: " << pair << endl;
      test.insert (pair);
   }

   for (str_str_map::iterator itor = test.begin();
        itor != test.end(); ++itor) {
      cout << "During iteration: " << *itor << endl;
   }

   str_str_map::iterator itor = test.begin();
   test.erase (itor);

   cout << "EXIT_SUCCESS" << endl;
   return EXIT_SUCCESS;
}

