// $Id: astree.cpp,v 1.15 2019-08-22 17:28:52-07 - - $

#include <cassert>
#include <iomanip>
#include <iostream>
using namespace std;

#include "astree.h"
#include "string_set.h"
#include "lyutils.h"

ostream& operator<< (ostream& out, const location& loc) {
   return out << "{" << loc.filenr << "," << loc.linenr << ","
              << loc.offset << "}";
}

ostream& operator<< (ostream& out, const astree* tree) {
   return out << parser::get_tname (tree->symbol)
              << " \"" << *tree->lexinfo << "\" " << tree->loc;
}

astree::astree (int symbol_, const location& loc_, const char* info):
                symbol (symbol_), loc (loc_),
                lexinfo (string_set::intern (info)) {
}

astree::~astree() {
   while (not children.empty()) {
      astree* child = children.back();
      children.pop_back();
      delete child;
   }
   if (yydebug) {
      cerr << "Deleting astree (";
      astree::dump (cerr, this);
      cerr << ")" << endl;
   }
}

astree* astree::adopt (astree* child1, astree* child2) {
   if (child1 != nullptr) children.push_back (child1);
   if (child2 != nullptr) children.push_back (child2);
   return this;
}

astree* astree::adopt_sym (astree* child, int symbol_) {
   symbol = symbol_;
   return adopt (child);
}


void astree::dump_node (ostream& out) {
   out << static_cast<const void*> (this) << "->{" << this;
   for (const auto& child: children) {
      out << " " << static_cast<const void*> (child);
   }
}

void astree::dump_tree (ostream& out, int depth) {
   out << setw (depth * 3) << "";
   dump_node (out);
   out << endl;
   for (astree* child: children) child->dump_tree (out, depth + 1);
}

void astree::dump (ostream& out, astree* tree) {
   if (tree == nullptr) out << "nullptr";
                   else tree->dump_node (out);
}

void astree::print (ostream& out, astree* tree, int depth) {
   out << "; AST: " << setw (depth * 3) << ""
       << tree << endl;
   for (astree* child: tree->children) {
      astree::print (out, child, depth + 1);
   }
}

void destroy (astree* tree1, astree* tree2) {
   if (tree1 != nullptr) delete tree1;
   if (tree2 != nullptr) delete tree2;
}

