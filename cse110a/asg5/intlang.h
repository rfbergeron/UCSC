#ifndef __INTLANG_H__
#define __INTLANG_H__

#include <string>

#include "auxlib.h"
#include "astree.h"
#include "lyutils.h"

using namespace std;

class generator {
    using symbol_table = unordered_map<const string*,symbol_value*>;
    using symbol_entry = symbol_table::value_type;
    private:
        static size_t string_count;
        static size_t branch_count;
        static const string TAB;
        static ostream* out;
    public:
        static void set_out(ostream out_);
        static int write_int_lang(astree* root,
                vector<symbol_table*> tables, vector<string> strings);
        static int write_var_decl(symbol_entry);
        static int write_string_decl(string);
        static int write_struct_decl(symbol_entry);
        static int write_function_decl(astree*,symbol_table*);
        static void write_type(symbol_value*);
        static void write_type(astree*);
};
#endif
