#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <iomanip>
#include <fstream>
#include <vector>

#include <libgen.h>
#include <unistd.h>

#include "auxlib.h"
#include "string_set.h"
#include "lyutils.h"
#include "astree.h"
#include "yyparse.h"

const string CPP = "/usr/bin/cpp";
const string OC_EXT = ".oc";
const string STR_EXT = ".str";
const string TOK_EXT = ".tok";
const string AST_EXT = ".ast";
const string SYM_EXT = ".sym";
constexpr size_t LINESIZE = 1024;
string cpp_opts = " -nostdinc";
ofstream strfile;
ofstream tokfile;
ofstream astfile;
ofstream symfile;

int main(int argc, char** argv) {
   yydebug = 0;
   yy_flex_debug = 0;
   scan_options(argc, argv);
   ios_base::sync_with_stdio(true);
   // calls to getopt increment optind, so after scan_options returns
   // we should be at the name of the target file.
   DEBUGH('a', "     oc program infile_path: " << argv[optind]);
   DEBUGH('a', "     CPP exec: " << CPP << cpp_opts);

   string oc_name { basename(argv[optind]) };
   size_t ext_index = oc_name.rfind(OC_EXT);
   if(ext_index == string::npos ||
         OC_EXT.compare(oc_name.substr(ext_index))) {
      cerr << "not an .oc file: " << oc_name << " " <<
         oc_name.substr(ext_index) << endl;
      return EXIT_FAILURE;
   }

   string base_name = oc_name.substr(0, ext_index);
   string str_name = base_name + STR_EXT;
   string tok_name = base_name + TOK_EXT;
   string ast_name = base_name + AST_EXT;
   string sym_name = base_name + SYM_EXT;

   strfile.open(str_name.c_str());
   if((strfile.rdstate() & ofstream::failbit) != 0) {
      cerr << "failed to open file for writing: "
            << str_name << endl;
      return EXIT_FAILURE;
   }

   tokfile.open(tok_name.c_str());
   if((tokfile.rdstate() & ofstream::failbit) != 0) {
      cerr << "failed to open file for writing: "
            << tok_name << endl;
      return EXIT_FAILURE;
   }

   astfile.open(ast_name.c_str());
   if((astfile.rdstate() & ofstream::failbit) != 0) {
      cerr << "failed to open file for writing: "
            << ast_name << endl;
      return EXIT_FAILURE;
   }

   symfile.open(sym_name.c_str());
   if((symfile.rdstate() & ofstream::failbit) != 0) {
      cerr << "failed to open file for writing: "
            << sym_name << endl;
      return EXIT_FAILURE;
   }

   string command = CPP + cpp_opts + " " + oc_name;
   yyin = popen(command.c_str(), "r");
   if(yyin == nullptr ) {
      cerr << "failed to open pipe for command: " << command << endl;
      return EXIT_FAILURE;
   } else {
      DEBUGH('y', "  activating bison");
      int parse_status = yyparse();

      if(parse_status == 0) {
         DEBUGH('y', "  bison parse was successful");
         // do type checking
         astree::print(astfile, parser::root, 0); 
      }
      else {
        cerr << "Parsing failed with status " << parse_status << endl;  
      }
      pclose_status = pclose(yyin);
      cerr_status (command.c_str(), pclose_status);
      strfile.close();
      tokfile.close();
      astfile.close();
      symfile.close();
      return EXIT_SUCCESS;
   }
}
