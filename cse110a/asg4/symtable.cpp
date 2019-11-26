#include "astree.h"
#include "lyutils.h"
#include "symtable.h"

using symbol_table = unordered_map<const string*,symbol_value*>;
using attr_bitset = bitset<static_cast<long unsigned int>(16)>;
symbol_table* type_checker::type_names = new symbol_table();
symbol_table* type_checker::globals = new symbol_table();
symbol_table* type_checker::locals = nullptr;
vector<symbol_table*> type_checker::local_tables;
int type_checker::next_block = 1;
attr_bitset type_checker::TYPE_ATTR_MASK;
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

ostream& operator<< (ostream& out, const symbol_value* symval) {
   out << symval->lloc << " {" << symval->block_nr << "}";
   for(size_t i = 1; i < (size_t)attr::BITSET_SIZE; ++i) {
      if(symval->attributes.test(i)) {
         if(i == (size_t)attr::STRUCT) {
            if(symval->fields == nullptr) {
                DEBUGH('s', "Printing struct type");
                out << " ptr <struct " << *(symval->type_id) << ">";
            } else {
                DEBUGH('s', "Printing " << static_cast<attr>(i)
                    << " type");
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
        size_t block_nr_):
        attributes(tree->attributes), lloc(tree->loc),
        type_id(tree->type_id), sequence(sequence_),
        block_nr(block_nr_) {
}

symbol_value::~symbol_value() {
    if(fields != nullptr) {
        for(auto iter = fields->begin(); iter != fields->end();
                ++iter) {
            delete iter->second;
        }
        delete fields;
    }
}

int type_checker::make_symbol_table(astree* root) {
    TYPE_ATTR_MASK.set((size_t)attr::INT).set((size_t)attr::VOID)
            .set((size_t)attr::STRING).set((size_t)attr::NULLPTR_T)
            .set((size_t)attr::STRUCT).set((size_t)attr::ARRAY);
    DEBUGH('t', "Making symbol table");
    for(astree* child : root->children) {
        switch(child->symbol) {
            case TOK_FUNCTION:
                make_function_entry(child);
                break;
            case TOK_STRUCT:
                make_structure_entry(child);
                break;
            case TOK_IDENT:
                make_global_entry(child);
                break;
            case '=':
                // global entry with assignment
                break;
            default:
                cerr << "ERROR: Unexpected symbol at top level: "
                     << parser::get_tname(child->symbol) << endl;
                return -1;
        }
    }
    return 0;
}

int type_checker::make_global_entry(astree* global) {
    if(globals->find(extract_ident(global)) != globals->end()) {
        //error; duplicate declaration
        return -1;
    } else {
        global->attributes.set((size_t)attr::LVAL);
        global->attributes.set((size_t)attr::VARIABLE);
        int status = validate_type_id(global);
        if(status != 0) return -1;
        symbol_value* global_value = new struct symbol_value(global);
        globals->insert({extract_ident(global), global_value});
        return 0;
    }
}

int type_checker::make_structure_entry(astree* structure) {
    const string* structure_type = structure->first()->lexinfo;
    DEBUGH('t', "Defining structure type: " << *structure_type);
    if(type_names->find(structure_type) != type_names->end()) {
        cerr << "ERROR: Duplicate definition of structure "
             << *structure_type << endl;
        return -1;
    } else {
        symbol_value* structure_value =
                new struct symbol_value(structure);
        symbol_table* fields = new symbol_table();
        type_names->insert({structure_type, structure_value});
        // start from 2nd child; 1st was type name
        for(size_t i = 1; i < structure->children.size(); ++i) {
            astree* field = structure->children[i];
            const string* field_id_str = extract_ident(field);
            DEBUGH('t', "Found structure field: " << *field_id_str);
            if(fields->find(field_id_str) != fields->end()) {
                cerr << "ERROR: Duplicate declaration of field: "
                     << *field_id_str << endl;
                return -1;
            } else {
                int status = validate_type_id(field);
                if(status != 0) return -1;
                symbol_value* field_value = new struct symbol_value(
                        field, i-1);
                fields->insert({field->lexinfo, field_value});
            }
        }
        structure_value->fields = fields;
    }
    return 0;
}

int type_checker::make_function_entry(astree* function) {
    int ret = 0;
    int status;
    astree* type_id = function->first();
    const string* function_id = extract_ident(type_id);
    status = validate_type_id(type_id);
    if(status != 0) return status;
    //DEBUGH('t', "Making entry for function: " << *function_id);
    if(type_id->attributes.test((size_t)attr::ARRAY)) {
        cerr << "ERROR: Function " << *function_id
             << " has an array return type, which is not allowed!"
             << endl;
        // TODO(rbergero): keep going if we can
        return -1;
    } else if(type_id->attributes.test((size_t)attr::STRUCT)) {
        if(type_names->find(function->type_id) == type_names->end()) {
            cerr << "ERROR: Structure has no definition: "
                 << function->type_id << endl;
            return -1;
        }
    }

    struct symbol_value* function_entry =
            new struct symbol_value(function);
    locals = new symbol_table();
    // check and add parameters
    set_block_nr(function->second(), next_block);
    astree* params = function->second();
    size_t param_sequence_nr = 0;
    for(size_t i = 0; i < params->children.size(); ++i) {
        astree* param = params->children[i];
        const string* param_id_str = extract_ident(param);
        if(locals->find(param_id_str) != locals->end()) {
            cerr << "ERROR: Duplicate declaration of parameter: "
                 << *param_id_str << endl;
            return -1;
        }
        astree* param_identifier = param->second();
        param_identifier->attributes.set((size_t)attr::PARAM);
        status = make_local_entry(param, param_sequence_nr);
        if(status != 0) return status;
        struct symbol_value* param_entry = new struct symbol_value(
                param, i, next_block);
        locals->insert({param_id_str,param_entry});
        function_entry->parameters.push_back(param_entry);
    }


    DEBUGH('t', "Inserting function entry with block id "
            << next_block);
    if(globals->find(function_id) != globals->end()) {
        symbol_value* prototype = globals->at(function_id);
        if(not functions_equal(prototype, function_entry)) {
            cerr << "ERROR: function has already been defined with a "
                 << "different return type/parameters: "
                 << function_id << endl;
            return -1;
        } else if(prototype->has_block) {
            cerr << "ERROR: function has already been defined: "
                 << function_id << endl;
            return -1;
        } else if(function->children.size() == 3) {
            size_t sequence_nr = 0;
            status = validate_block(function->third(), function_id,
                    sequence_nr);
            if(status != 0) return status;
            prototype->has_block = true;
        }
    } else { 
        globals->insert({function_id, function_entry});
        if(function->children.size() == 3) {
            size_t sequence_nr = 0;
            set_block_nr(function->third(), next_block);
            status = validate_block(function->third(), function_id,
                    sequence_nr);
            if(status != 0) return status;
            function_entry->has_block = true;
        }
    }

    local_tables.push_back(locals);
    locals = nullptr;
    ++next_block;
    return 0;
}

int type_checker::make_local_entry(astree* local, size_t& sequence_nr) {
    if(locals->find(extract_ident(local)) != locals->end()) {
        cerr << "ERROR: Duplicate declaration of variable: "
             << *extract_ident(local) << endl;
        return -1;
    } else {
        astree* type = local->first();
        astree* identifier = local->second();
        identifier->attributes.set((size_t)attr::LVAL);
        identifier->attributes.set((size_t)attr::VARIABLE);
        int status = validate_type_id(local);
        if(status != 0) return -1;
        symbol_value* local_value = new struct 
                symbol_value(local, sequence_nr, next_block);
        locals->insert({extract_ident(local), local_value});
        ++sequence_nr;
        return 0;
    }
}

int type_checker::validate_block(astree* block,
        const string* function_name, size_t& sequence_nr) {
    for(astree* statement : block->children) {
        int status = validate_statement(statement, function_name,
                sequence_nr);
        if (status != 0) return status;
    }
    return 0;
}

int type_checker::validate_statement(astree* statement,
        const string* function_name, size_t& sequence_nr) {
    for(astree* child : statement->children) {
        int status;
        switch(child->symbol) {
            case TOK_RETURN:
                //if(child->children->empty() != 
                break;
            case TOK_TYPE_ID:
                // local var decl
                break;
            case '=':
                // the above, but with assignment
                break;
            case TOK_IF:
                status = validate_expression(child->first());
                if(status != 0) return status;
                status = validate_statement(child->second(),
                        function_name, sequence_nr);
                if(status != 0) return status;
                if(child->children.size() == 3)
                    status = validate_statement(child->third(),
                            function_name, sequence_nr);
                if(status != 0) return status;
                break;
            case TOK_WHILE:
                status = validate_expression(child->first());
                if(status != 0) return status;
                status = validate_statement(child->second(),
                        function_name, sequence_nr);
                if(status != 0) return status;
                break;
            case TOK_BLOCK:
                validate_block(child, function_name, sequence_nr);
                break;
        }
    }
    return 0;
}

int type_checker::validate_expression(astree* expression) {
    for(astree* child : expression->children) {
        int status;
        const string* ident;
        switch(child->symbol) {
            case TOK_EQ:
            case TOK_NE:
            case TOK_LE:
            case TOK_GE:
            case TOK_GT:
            case TOK_LT:
            case '+':
            case '-':
            case '*':
            case '/':
            case '%':
                // handle int exprs
                status = validate_expression(child->first());
                if(status != 0) return status;
                status = validate_expression(child->second());
                if(status != 0) return status;
                if(!types_equal(child->first(), child->second())) {
                    return -1;
                } else if(!child->first()->attributes.test(
                        (size_t)attr::INT)) {
                    return -1;
                }
                break;
            case TOK_NOT:
            case TOK_POS:
            case TOK_NEG:
                // unary operators
                status = validate_expression(child->first());
                if(status != 0) return status;
                if(!child->first()->attributes.test((size_t)attr::INT))
                    return -1;
            case '=':
                // same as above but lhs must be an lval
                status = validate_expression(child->first());
                if(status != 0) return status;
                status = validate_expression(child->second());
                if(status != 0) return status;
                if(!types_equal(child->first(), child->second())) {
                    return -1;
                } else if(!child->first()->attributes.test(
                        (size_t)attr::LVAL)) {
                    return -1;
                }
                // type is the type of the left operand
                child->attributes = child->first()->attributes;
                break;
            case TOK_ALLOC:
                status = validate_type_id(child->first(), child);
                if(status != 0) return status;
                break;
            case TOK_CALL:
                status = validate_call(child);
                if(status != 0) return status;
                break;
            case TOK_INTCON:
            case TOK_STRINGCON:
            case TOK_NULLPTR:
            case TOK_CHARCON:
                // types are set on construction
                break;
            case TOK_IDENT:
                // get the type information
                status = assign_type(child);
                if(status != 0) return status;
                break;
            default:
                cerr << "ERROR: UNANTICIPATED SYMBOL: "
                     << *(child->lexinfo) << endl;
                return -1;
        }
    }
    return 0;
}

int type_checker::validate_type_id(astree* type_id) {
    return validate_type_id(type_id->first(), type_id->second());
}
int type_checker::validate_type_id(astree* type, astree* identifier) {
    if(type->symbol == TOK_ARRAY) {
        identifier->attributes.set((size_t)attr::ARRAY);
        type = type->first();
    }
    switch(type->symbol) {
        case TOK_INT:
            identifier->attributes.set((size_t)attr::INT);
            break;
        case TOK_STRING:
            identifier->attributes.set((size_t)attr::STRING);
            break;
        case TOK_PTR:
            identifier->attributes.set((size_t)attr::STRUCT);
            type->first()->attributes.set((size_t)attr::TYPEID);
            identifier->type_id = type->first()->lexinfo;
            if(type_names->find(identifier->type_id) ==
                    type_names->end()) {
                cerr << "ERROR: Type not declared: "
                     << *(identifier->type_id) << endl;
                return -1;
            }
            break;
        case TOK_VOID:
            if(identifier->attributes.test((size_t)attr::ARRAY)) {
                cerr << "ERROR: you may not have arrays of type void!"
                     << endl;
                return -1;
            } else if(identifier->attributes.test((size_t)attr::
                    FUNCTION)) {
                cerr << "ERROR: variables and fields cannot be of type "
                     << "void!" << endl;
                return -1;
            } else {
                identifier->attributes.set((size_t)attr::VOID);
            }
            break;
    }
    return 0;
}

int type_checker::validate_call(astree* call) {
    const string* identifier = call->first()->lexinfo;
    vector<astree*> params = call->second()->children;
    if(globals->find(identifier) != globals->end()) {
        symbol_value* function = globals->at(identifier);
        if(params.size() != function->parameters.size()) {
            cerr << "ERROR: incorrect number of arugments: "
                 << *identifier << endl;
            return -1;
        }
        for(size_t i = 0; i < params.size(); ++i) {
            astree* param = params[i];
            int status = assign_type(param);
            if(status != 0) return status;
            if(!types_equal(param, function->parameters[i])) {
                cerr << "ERROR: incompatible type for argument: "
                     << *(param->lexinfo) << endl;
                return -1;
            }
        }
        call->attributes = function->attributes;
        return 0;
    } else {
        cerr << "ERROR: Invalid call to function: "
             << *identifier << endl;
        return -1;
    }
}

int type_checker::assign_type(astree* ident) {
    const string* id_str = ident->lexinfo;
    if(locals->find(id_str) != locals->end()) {
        ident->attributes = locals->at(id_str)->attributes;
    } else if(globals->find(id_str) != globals->end()) {
        ident->attributes = globals->at(id_str)->attributes;
    } else {
        cerr << "ERROR: could not resolve symbol: "
             << *(ident->lexinfo) << endl;
        return -1;
    }
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
    return types_equal(v1->attributes, v2->attributes);
}

bool type_checker::types_equal(astree* t1, astree* t2) {
    return types_equal(t1->attributes, t2->attributes);
}

bool type_checker::types_equal(astree* tree, symbol_value* entry) {
    return types_equal(tree->attributes, entry->attributes);
}

bool type_checker::types_equal(attr_bitset a1, attr_bitset a2) {
    if(a1.test((size_t)attr::ARRAY) !=
            a2.test((size_t)attr::ARRAY)) {
        return false;
    } else if(a1.test((size_t)attr::VOID) &&
            a2.test((size_t)attr::VOID)) {
        return true;
    } else if(a1.test((size_t)attr::STRUCT) &&
            a2.test((size_t)attr::STRUCT)) {
        return true;
    } else if(a1.test((size_t)attr::STRING) &&
            a2.test((size_t)attr::STRING)) {
        return true;
    } else if(a1.test((size_t)attr::INT) && 
            a2.test((size_t)attr::INT)) {
        return true;
    } else {
        return false;
    }
}

astree* type_checker::extract_param(astree* function, size_t index) {
    return function->second()->children[index];
}

const string* type_checker::extract_ident(astree* type_id) {
    return type_id->second()->lexinfo;
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
            out << "   " << *(fiter->first) << " " << fiter->second
                << endl;
        }
    }
    DEBUGH('s', "Dumping global declarations");
    for(auto iter = globals->begin(); iter != globals->end(); ++iter) {
        out << *(iter->first) << " " << iter->second << endl;
        if(iter->second->attributes.test((size_t)attr::FUNCTION)) {
            DEBUGH('s', "Dumping local declarations for block "
                    << current_block_nr + 1);
            symbol_table* locals = local_tables[current_block_nr];
            for(auto liter = locals->begin(); liter != locals->end();
                    ++liter) {
                //if(liter->second->block_nr == current_block_nr) {
                    out << "   " << *(liter->first) << " "
                        << liter->second << endl;
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
        for(auto iter = locals->begin(); iter != locals->end();
                ++iter) {
            delete iter->second;
        }
        delete locals;
    }
    DEBUGH('t', "  SYMTABLES DESTROYED");
}
