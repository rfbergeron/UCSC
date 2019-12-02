#ifndef __INTLANG_H__
#define __INTLANG_H__

#include <string>

#include "auxlib.h"
#include "astree.h"
#include "lyutils.h"

using namespace std;

class generator {
    using symbol_pair = pair<const string*, symbol_value*>;
    using symbol_table = unordered_map<const string*,symbol_value*>;
    private:
        static size_t string_count;
        static size_t branch_count;
        static ofstream& out;
    public:
        static void set_out(ostream& out);
        static int write_int_lang(astree* root,
                symbol_table* structures, symbol_table* globals,
                vector<symbol_table*> locals, vector<string> strings);
        static int write_var_decl(symbol_pair);
        static int write_string_decl(string);
        static int write_struct_decl(symbol_pair);
        static int write_function_decl(astree*,symbol_table*);
        static void write_type(symbol_value*, ostream&);
        static void write_type(astree*, ostream&);
};
#endif
