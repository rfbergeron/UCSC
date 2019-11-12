// $Id: astree.cpp,v 1.15 2019-08-22 17:28:52-07 - - $

#include <cassert>
#include <iomanip>
#include <iostream>
using namespace std;

#include "astree.h"
#include "string_set.h"
#include "lyutils.h"

const char* astree::NOINFO = "";

ostream& operator<< (ostream& out, const location& loc) {
   return out << "{" << loc.filenr << "," << loc.linenr << ","
              << loc.offset << "}";
}

ostream& operator<< (ostream& out, const astree* tree) {
   const char* tname = parser::get_tname (tree->symbol);
   tname += 4;
   return out << tname << " \"" << *tree->lexinfo << "\" " << tree->loc;
}

astree::astree (int symbol_, const location& loc_, const char* info):
                symbol (symbol_), loc (loc_),
                lexinfo (string_set::intern (info)), firstborn(this),
                next_sibling(nullptr) {
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

astree* astree::adopt (astree* child1, astree* child2, astree* child3) {
   if (child1 != nullptr) { 
      astree* current_sibling = child1->firstborn;
      do {
         children.push_back(current_sibling);
         current_sibling = current_sibling->next_sibling;
      } while(current_sibling != nullptr);
   }
   if (child2 != nullptr) {
      astree* current_sibling = child2->firstborn;
      do {
         children.push_back(current_sibling);
         current_sibling = current_sibling->next_sibling;
      } while(current_sibling != nullptr);
   }
   if (child3 != nullptr) {
      astree* current_sibling = child3->firstborn;
      do {
         children.push_back(current_sibling);
         current_sibling = current_sibling->next_sibling;
      } while(current_sibling != nullptr);
   }
   return this;
}

astree* astree::adopt_sym (int symbol_, astree* child1, astree* child2) {
   symbol = symbol_;
   return adopt (child1, child2);
}

astree* astree::buddy_up (astree* sibling) {
   // if it is the head of the list, this node points to itself
   sibling->firstborn = firstborn;
   next_sibling = sibling;
   // want to append to the end of the "list"
   DEBUGH('y', "  buddying up " << parser::get_tname(symbol)
         << " with " << parser::get_tname(sibling->symbol)
         << "; oldest sib: " << parser::get_tname(firstborn->symbol)
         << " " << *(firstborn->lexinfo));
   return sibling;
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
   for(int i = 0; i < depth; ++i) {
      out << "|  ";
   }
   out << tree << endl;
   for (astree* child: tree->children) {
      astree::print (out, child, depth + 1);
   }
}

void destroy (astree* tree1, astree* tree2, astree* tree3) {
   DEBUGH('y', "  DESTROYING");
   if (tree1 != nullptr) delete tree1;
   if (tree2 != nullptr) delete tree2;
   if (tree3 != nullptr) delete tree3;
}

