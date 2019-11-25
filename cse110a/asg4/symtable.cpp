#include "astree.h"
#include "lyutils.h"
#include "symtable.h"

using symbol_table = unordered_map<const string*,symbol_value*>;
using attr_bitset = bitset<static_cast<long unsigned int>(16)>;
symbol_table* type_checker::type_names = new symbol_table();
symbol_table* type_checker::globals = new symbol_table();
vector<symbol_table*> type_checker::local_tables;
int type_checker::next_block = 1;
const attr_bitset type_checker::TYPE_ATTR_MASK;
const unordered_map<attr,const string> type_checker::attr_map= {
      {attr::VOID,      "void"     },
      {attr::INT,       "int"      },
      {attr::NULLPTR_T, "nullptr"  },
      {attr::STRING,    "string"   },
      {attr::STRUCT,    "struct"   },
      {attr::ARRAY,     "array"    },
      {attr::FUNCTION,  "function" },
      {attr::VARIABLE,  "variable" },
      {attr::FIELD,     "field"    },
      {attr::TYPEID,    "typeid"   },
      {attr::PARAM,     "param"    },
      {attr::LOCAL,     "local"    },
      {attr::LVAL,      "lval"     },
      {attr::CONST,     "const"    },
      {attr::VREG,      "vreg"     },
      {attr::VADDR,     "vaddr"    }};

ostream& operator<< (ostream& out, const attr& attribute) {
   return out << type_checker::attr_map.at(attribute);
}

/*ostream& operator<< (ostream&out, const attr_bitset& attributes) {
    if(attributes.test(0)) out << static_cast<attr>(0);
    for(size_t i = 1; i < (size_t)attr::BITSET_SIZE; ++i) {
        if(attributes.test(i)) out << " " << static_cast<attr>(i);
    }
    return out;
}*/

ostream& operator<< (ostream& out, const symbol_value* symval) {
   out << symval->lloc << " {" << symval->block_nr << "}";
   for(size_t i = 1; i < (size_t)attr::BITSET_SIZE; ++i) {
      if(symval->attributes.test(i)) {
         if(i == (size_t)attr::STRUCT) {
            if(symval->fields == nullptr) {
                DEBUGH('s', "Printing struct type");
                out << " ptr <struct " << *(symval->type_id) << ">";
            } else {
                DEBUGH('s', "Printing " << static_cast<attr>(i) << " type");
                out << " " << static_cast<attr>(i) << " "
                    << *(symval->type_id);
            }
         } else {
            out << " " << static_cast<attr>(i);
         }
      }
   }
   return out;
}

symbol_value::symbol_value(astree* tree, size_t sequence_,
        size_t block_nr_, vector<symbol_value*> parameters_,
        symbol_table* fields_):
        attributes(tree->attributes), lloc(tree->loc),
        type_id(tree->type_id), sequence(sequence_),
        block_nr(block_nr_), parameters(parameters_), fields(fields_) {
}

symbol_value::~symbol_value() {
    if(fields != nullptr) {
        for(auto iter = fields->begin(); iter != fields->end(); ++iter) {
            delete iter->second;
        }
        delete fields;
    }
}

int type_checker::make_symbol_table(astree* root) {
    int ret = 0;
    DEBUGH('t', "Making symbol table");
    for(astree* child : root->children) {
        if(child->attributes.test((size_t)attr::FUNCTION)) {
            // handle function definition
            make_function_entry(child);
        } else if(child->attributes.test((size_t)attr::VREG)) {
            // handle global identifier declaration, with assignment
        } else if(child->children[0]->lexinfo->compare("struct")) {
            // handle structure definition
            make_structure_entry(child);
        } else {
            // global identifier, no assignment. That's the only other
            // thing the parser should be letting through at the top
            make_global_entry(child);
        }
    }
    return ret;
}

int type_checker::make_global_entry(astree* global) {
    if(globals->find(extract_ident(global)) != globals->end()) {
        //error; duplicate declaration
        return -1;
    } else {
        global->attributes.set((size_t)attr::LVAL);
        global->attributes.set((size_t)attr::VARIABLE);
        symbol_value* global_value = new symbol_value(global);
        globals->insert({extract_ident(global), global_value});
        return 0;
    }
}

int type_checker::make_structure_entry(astree* structure) {
    const string* structure_type = structure->children[0]->lexinfo;
    int ret = 0;
    DEBUGH('t', "Defining structure type: " << *structure_type);
    if(type_names->find(structure_type) != type_names->end()) {
        // error; duplicate declaration
        ret = -1;
    } else {
        symbol_value* structure_value = new symbol_value(structure);
        structure_value->fields = new symbol_table();
        for(size_t i = 1; i < structure->children.size(); ++i) {
            astree* field_node = structure->children[i];
            DEBUGH('t', "Found structure field: " <<
                    *extract_ident(field_node));
            if(structure_value->fields->find(extract_ident(field_node))
                    != structure_value->fields->end()) {
                // error; duplicate field names
                ret = -1;
            } else {
                symbol_value* field_value = new struct symbol_value(
                        field_node, i-1);
                // maybe need to set sequence # for fields
                structure_value->fields->insert({field_node->lexinfo,
                        field_value});
            }
        }
        type_names->insert({structure_type, structure_value});
    }
    return ret;
}

int type_checker::make_function_entry(astree* function) {
    int ret = 0;

    const string* function_id = extract_ident(function->children[0]);
    //DEBUGH('t', "Making entry for function: " << *function_id);
    if(function->attributes.test((size_t)attr::ARRAY)) {
        // error, functions not allowed to return arrays
        cerr << "ERROR: Function " << *function_id
             << " has an array return type, which is not allowed!"
             << endl;
        // TODO: keep going if we can
        //ret = -1;
        return -1;
    }
    if(function->attributes.test((size_t)attr::STRUCT)) {
        if(type_names->find(function->type_id) == type_names->end()) {
            // error; structure not defined
            cerr << "ERROR: Structure has no definition: "
                 << function->type_id << endl;
            //ret = -1;
            return -1;
        }
    }

    struct symbol_value* function_entry =
            new struct symbol_value(function);
    symbol_table* locals = new symbol_table();
    // check and add parameters
    set_block_nr(function->children[1], next_block);
    for(size_t i = 0; i < function->children[1]->children.
            size(); ++i) {
        astree* param_node = extract_param(function, i);
        const string* param_ident = extract_ident(param_node);
        //DEBUGH('t', "Making entry for parameter " << *param_ident);
        if(locals->find(param_ident) != locals->end()) {
        // TODO: add location of previous declaration
            cerr << "ERROR: Duplicate declaration of " << *param_ident << endl;
            return -1;
        }
        param_node->attributes.set((size_t)attr::VARIABLE);
        param_node->attributes.set((size_t)attr::LVAL);
        param_node->attributes.set((size_t)attr::PARAM);
        struct symbol_value* param_entry = new struct symbol_value(
                param_node, i, next_block);
        locals->insert({param_ident,param_entry});
        function_entry->parameters.push_back(param_entry);
    }


    DEBUGH('t', "Inserting function entry with block id " << next_block);
    globals->insert({function_id, function_entry});
    local_tables.push_back(locals);
    ++next_block;
    return 0;
}

bool type_checker::functions_equal(symbol_value* f1, symbol_value* f2) {
    if(f1->parameters.size() != f2->parameters.size())
        return false;
    for(size_t i = 0; i < f1->parameters.size(); ++i) {
        if(!types_equal(f1->parameters[i], f2->parameters[i]))
            return false;
    }
    return true;
}

bool type_checker::types_equal(symbol_value* v1, symbol_value* v2) {
    if(v1->attributes.test((size_t)attr::ARRAY) !=
            v2->attributes.test((size_t)attr::ARRAY)) {
        return false;
    } else if(v1->attributes.test((size_t)attr::VOID) &&
            v2->attributes.test((size_t)attr::VOID)) {
        return true;
    } else if(v1->attributes.test((size_t)attr::STRUCT) &&
            v2->attributes.test((size_t)attr::STRUCT)) {
        return true;
    } else if(v1->attributes.test((size_t)attr::STRING) &&
            v2->attributes.test((size_t)attr::STRING)) {
        return true;
    } else if(v1->attributes.test((size_t)attr::INT) &&
            v2->attributes.test((size_t)attr::INT)) {
        return true;
    } else {
        return false;
    }
}

astree* type_checker::extract_param(astree* function, size_t index) {
    return function->children[1]->children[index];
}

const string* type_checker::extract_ident(astree* type_id) {
    return type_id->children[1]->lexinfo;
}

attr type_checker::get_type_attr(const symbol_value* symval) {
    attr_bitset attributes = symval->attributes;
    if(attributes.test((size_t)attr::VOID)) return attr::VOID;
    if(attributes.test((size_t)attr::STRUCT)) return attr::STRUCT;
    if(attributes.test((size_t)attr::INT)) return attr::INT;
    if(attributes.test((size_t)attr::STRING)) return attr::STRING;
    return attr::NULLPTR_T;
}

void type_checker::set_block_nr(astree* tree, size_t nr) {
    tree->block_nr = nr;
    for(auto iter = tree->children.begin(); iter != tree->
            children.end(); ++iter) {
        set_block_nr(*iter, nr);
    }
}

void type_checker::dump_symbols(ostream& out) {
    size_t current_block_nr = 0;
    DEBUGH('s', "Dumping structure types");
    for(auto iter = type_names->begin(); iter != type_names->end();
            ++iter) {
        out << iter->second << endl;
        for(auto fiter = iter->second->fields->begin(); fiter !=
                iter->second->fields->end(); ++fiter) {
            out << "   " << *(fiter->first) << " " << fiter->second << endl;
        }
    }
    DEBUGH('s', "Dumping global declarations");
    for(auto iter = globals->begin(); iter != globals->end(); ++iter) {
        out << *(iter->first) << " " << iter->second << endl;
        if(iter->second->attributes.test((size_t)attr::FUNCTION)) {
            DEBUGH('s', "Dumping local declarations for block " << current_block_nr + 1);
            symbol_table* locals = local_tables[current_block_nr];
            for(auto liter = locals->begin(); liter != locals->end(); ++liter) {
                //if(liter->second->block_nr == current_block_nr) {
                    out << "   " << *(liter->first) << " " << liter->second << endl;
                //}
            }
            ++current_block_nr;
            out << endl;
        }
    }
}

void type_checker::destroy_tables() {
    DEBUGH('t', "  DESTROYING TYPE NAME SYMTABLE");
    for(auto iter = type_names->begin(); iter != type_names->end();
            ++iter) {
        delete iter->second;
    }
    delete type_names;
    DEBUGH('t', "  DESTROYING GLOBAL SYMTABLE");
    for(auto iter = globals->begin(); iter != globals->end(); ++iter) {
        delete iter->second;
    }
    delete globals;
    DEBUGH('t', "  DESTROYING LOCAL SYMTABLES");
    for(auto local_iter = local_tables.begin(); local_iter !=
            local_tables.end(); ++local_iter) {
        symbol_table* locals = *local_iter;
        for(auto iter = locals->begin(); iter != locals->end(); ++iter) {
            delete iter->second;
        }
        delete locals;
    }
    DEBUGH('t', "  SYMTABLES DESTROYED");
}
