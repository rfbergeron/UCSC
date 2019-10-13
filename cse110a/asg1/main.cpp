#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iomanip>

#include "auxlib.h"

const string CPP = "/usr/bin/cpp";
constexpr size_t LINESIZE = 1024;
string cpp_opts = " -nostdinc";
int yy_flex_debug = 0;
int yydebug = 0;

// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}

// Print the meaning of a signal.
static string sig_text (int signal) {
   string sig_nr = to_string (signal);
   const char* sigstr = strsignal (signal);
   if (sigstr == nullptr) return sig_nr;
                     else return sig_nr + " (" + sigstr + ")";
}

// Print the status returned from a subprocess.
void cerr_status (const char* command, int status) {
   if (status == 0) return; 
   cerr << command << ": " << status << setw(4) << setfill('0')
        << hex << status << setfill(' ') << dec;
   if (WIFEXITED (status)) {
      cerr << ", exit " << WEXITSTATUS (status);
   }
   if (WIFSIGNALED (status)) {
      cerr << ", Terminated signal " << sig_text (WTERMSIG (status));
      #ifdef WCOREDUMP
      if (WCOREDUMP (status)) cerr << ", core dumped";
      #endif
   }
   if (WIFSTOPPED (status)) {
      cerr << ", Stopped signal ", sig_text (WSTOPSIG (status));
   }
   if (WIFCONTINUED (status)) {
      cerr << ", Continued";
   }
   cerr << endl;
}

// Run cpp against the lines of the file.
void cpplines (FILE* pipe, const string& filename) {
   int linenr = 1;
   for (;;) {
      char buffer[LINESIZE];
      const char* fgets_rc = fgets (buffer, LINESIZE, pipe);
      if (fgets_rc == nullptr) break;
      chomp (buffer, '\n');
      cout << filename << ":line " << linenr << ": [" << buffer
           << "]" << endl;
      // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
      char inputname[LINESIZE];
      int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                              &linenr, inputname);
      if (sscanf_rc == 2) {
         cout << "DIRECTIVE: line " << linenr << " file \""
              << inputname << "\"" << endl;
         continue;
      }
      char* savepos = nullptr;
      char* bufptr = buffer;
      for (int tokenct = 1;; ++tokenct) {
         char* token = strtok_r (bufptr, " \t\n", &savepos);
         bufptr = nullptr;
         if (token == nullptr) break;
         cout << "token " << linenr << "." << tokenct << ": ["
              << token << "]" << endl;
      }
      ++linenr;
   }
}

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
            DEBUGH('s', "cpp option: " << optarg);
            cpp_opts.append(" -D ").append(optarg);
            break;
         case 'l':
            DEBUGH('s', "yy_flex_debug set to 1");
            yy_flex_debug = 1;
            break;
         case 'y':
            DEBUGH('s', "yydebug set to 1");
            yydebug = 1;
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
   ios_base::sync_with_stdio(true);
   // calls to getopt increment optind, so after scan_options returns
   // we should be at the name of the target file.
   DEBUGH('a', "oc program filename: " << argv[optind]);
   DEBUGH('a', "CPP exec: " << CPP << cpp_opts);
   string filename = argv[optind];
   string command = CPP + cpp_opts + " " + filename;
   FILE* pipe = popen (command.c_str(), "r");
   if(pipe == nullptr) {
      cerr << "oof" << endl;
      return EXIT_FAILURE;
   } else {
      cpplines(pipe, filename);
      int pclose_status = pclose(pipe);
      cerr_status (command.c_str(), pclose_status);
   }
   return EXIT_SUCCESS;
}
