#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <cstddef>
#include <bitset>
#include <unordered_map>
#include <string>

using namespace std;

#include "auxlib.h"
#include "astree.h"
#include "lyutils.h"

using attr_bitset = bitset<attr::BITSET_SIZE>; 
using symbol_table = unordered_map<string*,symbol*>;
using symbol_entry = symbol_table::value_type;

enum class attr {
j   VOID, INT, NULLPTR_T, STRING, STRUCT, ARRAY, FUNCTION, VARIABLE,
    FIELD, TYPEID, PARAM, LOCAL, LVAL, CONST, VREG, VADDR, BITSET_SIZE,
};

struct symbol {
    attr_bitset attributes;
    size_t sequence;
    unordered_map<string*,symbol*>* fields;
    location lloc;
    size_t block_nr;
    vector<symbol*> parameters;
};

class type_checker {
    private:
        static symbol_table type_names;
        static symbol_table globals;
        static symbol_table locals;
        static int next_block = 0;
    public:
        static int make_symbol_table(astree*);
        static int make_function_entry(astree*);
        static int make_structure_entry(astree*);
        static int make_global_entry(astree*);
        static void dump_symbol_table();
};
#endif
