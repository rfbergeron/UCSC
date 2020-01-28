// John Gnanasekaran(jgnanase) and Robert Bergeron (rbergero)
// $Id: commands.cpp,v 1.17 2018-01-25 14:02:55-08 - - $

// Include for file input/output for
#include <fstream>
#include <iostream>
// Included for exit
#include <ctype.h>

#include <string>
#include <vector>

#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
      {"cat", fn_cat},
      {"cd", fn_cd},
      {"echo", fn_echo},
      {"exit", fn_exit},
      {"ls", fn_ls},
      {"lsr", fn_lsr},
      {"make", fn_make},
      {"mkdir", fn_mkdir},
      {"prompt", fn_prompt},
      {"pwd", fn_pwd},
      {"rm", fn_rm},
      {"#", fn_string},
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end ()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
      runtime_error (what) {}

// Function to print # comments
void fn_string (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // Return the string back with the # character
   cout << "# " << word_range (words.cbegin () + 1, words.cend ())
        << endl;
}

int exit_status_message () {
   int exit_status = exit_status::get ();
   cout << execname () << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

void fn_cat (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // inode* ptr = state.get_cwd().get();
   // base_file* bf = ptr->get_contents().get();
   // vector<string> contents = bf->readfile(words.at(1));
   // cout << word_range (contents.cbegin(), contents.cend()) << endl;
}

void fn_cd (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   // Prints out everything after echo
   cout << word_range (words.cbegin () + 1, words.cend ()) << endl;
}

void fn_exit (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if (words.empty ()) {
      exit_status::set (0);
   } else if (!words.empty ()) {
      string status;
      // Converts word vec to string
      for (auto const& str : words) {
         status += str + " ";
      }
      if (isdigit (status[5])) {
         // status is a number
         int integer_status =
               stoi (status.substr (5, status.length ()));
         exit_status::set (integer_status);
      } else if (isalpha (status[5])) {
         exit_status::set (127);
      }
   }
   throw ysh_exit ();
}

void fn_ls (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_lsr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_mkdir (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_prompt (inode_state& state, const wordvec& words) {
   string new_prompt;
   for (auto const& str : words) {
      new_prompt += str + " ";
   }
   new_prompt = new_prompt.substr (7, new_prompt.length ());
   state.set_prompt (new_prompt);
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_pwd (inode_state& state, const wordvec& words) {
   // Print pwd on a newline
   cout << endl << state.get_cwd ();
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rm (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words) {
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}
