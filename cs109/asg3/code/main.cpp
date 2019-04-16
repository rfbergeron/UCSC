// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $
// John Gnanasekaran(jgnanase) and Robert Bergeron (rbergero)

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
   regex comment_regex {R"(^\s*(#.*)?$)"};
   regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
   regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};
   // since istream is an ancestor of std::cin and can
   // be instantiated so long as we have a filebuf,
   // we can create an istream pointer and tell it to
   // point at cin if the user does not provide any arguments
   //
   // pointer was necessary since istream has no copy
   // constructor. also im not very knowledgeable about this
   // but copying std::cin sounds like a bad idea even if it 
   // is possible
   filebuf file;
   istream * input = nullptr;
   string input_line;
   str_str_map the_map;
   smatch result;
   
   // for loop doesnt terminate when i == argc to allow for
   // the loop to continue when argc == 1, in which case cin
   // will be read
   for(int i = 1 ; i <= argc ; i++) {
      if(i == argc) {
         if(i > 1) break;
         DEBUGF('y', "reading from cin now\n");
         input = &cin;
         input->ignore();
      }
      else {
         // needed to add extra if statement since for loop
         // doesnt terminate when i == argc
         DEBUGF('a', "opening file " << argv[i]);
         file.open(argv[i], ios::in);
         input = new istream(&file);
      }
      
      string input_name = argc == 1 ? "-" : argv[i];
      unsigned int line_number = 1;

      for(;;) {
         getline(*input, input_line);
         if((*input).eof()) {
            input->ignore();
            if(file.is_open()) {
               DEBUGF('a', "closing" << argv[i]);
               file.close();
               delete input;
            }
            DEBUGF('a', "moving to next file");
            break;
         }
         DEBUGF('a', "moving on to line " << line_number);

         cout << input_name << ": " << line_number 
            << ": " << input_line << endl;

         if (regex_search (input_line, result, comment_regex)) {
            DEBUGF('y', "Comment or empty line.");
         }
         else if (regex_search (input_line, result, trimmed_regex)) {
            str_str_map::iterator iter = the_map.find(result[1]);
            if(iter != the_map.end()) {
               cout << *iter << endl;
            }
            else {
               cout << result[1] << ": value not found" << endl;
            }
         }
         else if (regex_search (input_line, result, key_value_regex)) {
            str_str_pair pair (result[1], result[2]);

            if(result[1] == "") {
               str_str_map::iterator iter = the_map.begin();
               if(result[2] == "") {
                  // exectues if current line is exactly "="
                  // should print all keys in map
                  while(iter != the_map.end()) {
                     cout << *iter << endl;
                     ++iter;
                  }
               }
               else {
                  // executes if current line is of the form "=b"
                  // should print all keys with value b to cout
                  while(iter != the_map.end()) {
                     if((*iter).second == result[2]) {
                        cout << *iter << endl;
                     }
                     ++iter;
                  }
               }
            }
            else if(result[2] == ""){
               // executes if current line is of the form "a="
               // should delete key-value pair a, if it exists
               str_str_map::iterator loc = the_map.find(result[1]);
               if(loc != the_map.end()) {
                  the_map.erase(loc);
               }
            }
            else {
               // exectues if current line is of the form "a=b"
               // should insert new or reassign existing key
               cout << result[1] << " = " << result[2] << endl;
               the_map.insert(pair);
            }
         }
         ++line_number;
      }
   }

   cout << "EXIT_SUCCESS" << endl;
   return EXIT_SUCCESS;
}
