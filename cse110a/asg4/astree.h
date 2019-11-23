// $Id: astree.h,v 1.5 2019-08-22 17:23:59-07 - - $

#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

#include "auxlib.h"
#include "lyutils.h"

struct symbol_value;
enum class attr: long unsigned int;

struct location {
   size_t filenr {};
   size_t linenr {};
   size_t offset {};
};

ostream& operator<< (ostream&, const location&);

struct astree {
   using attr_bitset =
           bitset<static_cast<long unsigned int>(16)>; 
   using symbol_table = unordered_map<string*,symbol_value*>;
   using symbol_entry = symbol_table::value_type;

   // Fields.
   int symbol;                   // token code
   location loc;                 // source location
   const string* lexinfo;        // pointer to lexical information
   vector<astree*> children;     // children of this n-way node
   astree* next_sibling;         // for adopting long lists of siblings
   astree* firstborn;            // head of the list of siblings
   size_t block_nr = 0;          // block number this node occurs in
   attr_bitset attributes;       // type attributes
   symbol_table* structure_fields = nullptr; // structure fields
   static const char* NOINFO;    // use indicates no lexical info

   // Functions.
   astree (int symbol, const location&, const char* lexinfo);
   ~astree();
   astree* adopt (astree* child1, astree* child2 = nullptr,
         astree* child3 = nullptr);
   astree* adopt_sym (int symbol, astree* child1,
         astree* child2 = nullptr);
   astree* adopt_attributes (astree* child);
   astree* buddy_up (astree* sibling);
   void dump_node (ostream&);
   void dump_tree (ostream&, int depth = 0);
   static void dump (ostream& out, astree* tree);
   static void print (ostream& out, astree* tree, int depth = 0);
};

ostream& operator<< (ostream&, const astree*);

void destroy (astree* tree1, astree* tree2 = nullptr,
      astree* tree3 = nullptr);

#endif

