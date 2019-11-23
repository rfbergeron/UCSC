#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <cstdint>
#include <bitset>
#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

#include "auxlib.h"
#include "astree.h"
#include "lyutils.h"

// circular dependency with astree; forward declare
struct astree;
struct location;

enum class attr : long unsigned int {
    VOID, INT, NULLPTR_T, STRING, STRUCT, ARRAY, FUNCTION, VARIABLE,
    FIELD, TYPEID, PARAM, LOCAL, LVAL, CONST, VREG, VADDR, BITSET_SIZE,
};

struct symbol_value {
    using attr_bitset = bitset<
            static_cast<long unsigned int>(attr::BITSET_SIZE)>;
    using symbol_table = unordered_map<string*,symbol_value*>;
    using symbol_entry = symbol_table::value_type;

    attr_bitset attributes;
    size_t sequence;
    symbol_table* fields;
    //struct location lloc;
    size_t block_nr;
    vector<symbol_value*> parameters;
};

class type_checker {
    private:
        using attr_bitset = bitset<
                static_cast<long unsigned int>(attr::BITSET_SIZE)>; 
        using symbol_table = unordered_map<string*,symbol_value*>;
        using symbol_entry = symbol_table::value_type;

        static symbol_table type_names;
        static symbol_table globals;
        static symbol_table locals;
        static int next_block;
    public:
        static int make_symbol_table(astree*);
        static int make_structure_entry(astree*);
        static int make_function_entry(astree*);
        static int make_global_entry(astree*);
        static int make_local_entry(astree*);
        static int validate_block(astree*);
        static int validate_statement(astree*, const string*);
        static int validate_expression(astree*);
        static void dump_symbol_table();
};
#endif
