#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <bitset>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

#include "astree.h"
#include "auxlib.h"
#include "lyutils.h"

// circular dependency with astree; forward declare
struct astree;
struct location;

enum class attr : long unsigned int {
   VOID,
   INT,
   NULLPTR_T,
   STRING,
   STRUCT,
   ARRAY,
   FUNCTION,
   VARIABLE,
   FIELD,
   TYPEID,
   PARAM,
   LOCAL,
   LVAL,
   CONST,
   VREG,
   VADDR,
   BITSET_SIZE,
};

using attr_bitset = bitset<static_cast<long unsigned int> (16)>;
ostream& operator<< (ostream&, const attr&);
ostream& operator<< (ostream&, const attr_bitset&);

enum class type_err {
   NODECL,
   REDECL,
   BADTYPE,
   NOARR,
   BADRET,
};

struct symbol_value {
   using attr_bitset =
         bitset<static_cast<long unsigned int> (attr::BITSET_SIZE)>;
   using symbol_table = unordered_map<const string*, symbol_value*>;
   using symbol_entry = symbol_table::value_type;

   // Fields.
   attr_bitset attributes;
   size_t sequence = 0;
   symbol_table* fields = nullptr;
   struct location lloc;
   size_t block_nr = 0;
   vector<symbol_value*> parameters;
   const string* type_id = nullptr;
   bool has_block = false;

   // Functions
   symbol_value (astree* tree,
                 size_t sequence_ = 0,
                 size_t block_nr_ = 0);
   ~symbol_value ();
};

ostream& operator<< (ostream&, const symbol_value*);

class type_checker {
   private:
   using attr_bitset =
         bitset<static_cast<long unsigned int> (attr::BITSET_SIZE)>;
   using symbol_table = unordered_map<const string*, symbol_value*>;
   using symbol_entry = symbol_table::value_type;

   static symbol_table* type_names;
   static symbol_table* globals;
   static symbol_table* locals;
   static vector<symbol_table*> local_tables;
   static const unordered_map<type_err, const string> type_errs;
   static int next_block;
   static const string DUMMY_FUNCTION;
   static attr_bitset TYPE_ATTR_MASK;

   public:
   static const unordered_map<attr, const string> attr_map;
   static int make_symbol_table (astree*);
   static int make_structure_entry (astree*);
   static int make_function_entry (astree*);
   static int make_global_entry (astree*);
   static int make_local_entry (astree*, size_t&);
   static int validate_block (astree*, const string*, size_t&);
   static int validate_stmt_expr (astree* stmtexpr,
                                  const string* function_name,
                                  size_t&);
   static int validate_type_id (astree* type_id);
   static int validate_type_id (astree* type, astree* identifier);
   static int validate_call (astree*);
   static int assign_type (astree* ident);
   static int assign_type (astree* ident, symbol_value* value);
   static int assign_type (astree* dest, astree* source);
   static bool functions_equal (symbol_value* f1, symbol_value* f2);
   static bool types_equal (symbol_value*, symbol_value*);
   static bool types_equal (astree*, astree*);
   static bool types_equal (astree*, symbol_value*);
   static bool types_equal (attr_bitset, attr_bitset);
   static astree* extract_param (astree* function, size_t index);
   static const string* extract_ident (astree* type_id);
   static attr get_type_attr (const symbol_value* symval);
   static void set_block_nr (astree* tree, size_t nr);
   static void dump_symbols (ostream& out);
   static void symbol_error (astree*, type_err);
   static void destroy_tables ();
};
#endif
