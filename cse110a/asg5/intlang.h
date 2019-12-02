#ifndef __INTLANG_H__
#define __INTLANG_H__

#include "auxlib.h"
#include "astree.h"
#include "lyutils.h"

using namespace std;

class generator {
    private:
        static size_t string_count;
        static size_t branch_count;
        static ofstream& out;
    public:
        static void set_out(ostream& out);
        static int write_alloc(astree*);
        static int write_var_decl(astree*);
        static int write_struct_decl(astree*);
};
#endif
