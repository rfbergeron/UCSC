#include "astree.h"
#include "lyutils.h"
#include "symtable.h"
#include "auxlib.h"

using symbol_table = unordered_map<const string*,symbol_value*>;
using attr_bitset = bitset<static_cast<long unsigned int>(16)>;
symbol_table* type_checker::type_names = new symbol_table();
symbol_table* type_checker::globals = new symbol_table();
symbol_table* type_checker::locals = nullptr;
vector<symbol_table*> type_checker::local_tables;
vector<string> type_checker::string_constants;
int type_checker::next_block = 1;
const string type_checker::DUMMY_FUNCTION = "__DUMMY__";
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

ostream& operator<< (ostream& out, const attr_bitset& attributes) {
   if(attributes.test(0)) out << static_cast<attr>(0);
   for(size_t i = 1; i < (size_t)attr::BITSET_SIZE; ++i) {
      if(attributes.test(i)) {
         out << " " << static_cast<attr>(i);
      }
   }
   return out;
}

ostream& operator<< (ostream& out, const symbol_value* symval) {
   out << symval->lloc << " {" << symval->block_nr << "}";
   for(size_t i = 0; i < (size_t)attr::BITSET_SIZE; ++i) {
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

bool symbol_value::has_attr(attr attribute) {
    return attributes.test((size_t)attribute);
}

int type_checker::make_symbol_table(astree* root) {
    TYPE_ATTR_MASK.set((size_t)attr::INT).set((size_t)attr::VOID)
            .set((size_t)attr::STRING).set((size_t)attr::NULLPTR_T)
            .set((size_t)attr::STRUCT).set((size_t)attr::ARRAY);
    DEBUGH('t', "Making symbol table");
    for(astree* child : root->children) {
        int status;
        // validation method requires a sequence number but expressions
        // as defined by the parser cannot have vardecls below them in
        // the tree so we don't actually need to track sequence once we
        // get down that far, even though much of the code is the same
        size_t dummy_sequence = 0;
        switch(child->symbol) {
            case TOK_FUNCTION:
                status = make_function_entry(child);
                if(status != 0) return status;
                break;
            case TOK_STRUCT:
                status = make_structure_entry(child);
                if(status != 0) return status;
                break;
            case TOK_TYPE_ID:
                status = make_global_entry(child);
                if(status != 0) return status;
                break;
            case '=':
                status = make_global_entry(child->first());
                if(status != 0) return status;
                status = validate_stmt_expr(child->second(),
                        &DUMMY_FUNCTION, dummy_sequence);
                if(status != 0) return status;
                if(!types_equal(child->first(), child->second())) {
                    cerr << "ERROR: Incompatible types for global"
                         << endl;
                    return -1;
                } else if(!child->first()->attributes.test(
                        (size_t)attr::LVAL)) {
                    cerr << "ERROR: Global assignment destination"
                         << " is not an LVAL" << endl;
                    return -1;
                }
                // type is the type of the left operand
                child->attributes = child->first()->attributes;
                if(status != 0) return status;
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
        cerr << "ERROR: Global var has already been declared: "
             << *(extract_ident(global)) << endl;
        return -1;
    } else {
        astree* type = global->first();
        astree* identifier = global->second();
        identifier->attributes.set((size_t)attr::LVAL);
        identifier->attributes.set((size_t)attr::VARIABLE);
        int status = validate_type_id(global);
        if(status != 0) return status;
        symbol_value* global_value = new struct symbol_value(
                identifier);
        globals->insert({identifier->lexinfo, global_value});
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
                new struct symbol_value(structure->first());
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
                if(status != 0) return status;
                field->second()->attributes.set((size_t)attr::FIELD);
                field->second()->attributes.set((size_t)attr::LVAL);
                symbol_value* field_value = new struct symbol_value(
                        field->second(), i-1);
                fields->insert({field->second()->lexinfo, field_value});
                DEBUGH('t', "Field inserted at "
                        << *(field->second()->lexinfo));
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
    type_id->second()->attributes.set((size_t)attr::FUNCTION);
    const string* function_id = extract_ident(type_id);
    status = validate_type_id(type_id);
    if(status != 0) return status;
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
            new struct symbol_value(type_id->second());
    locals = new symbol_table();
    // check and add parameters; set block
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
                param->second(), i, next_block);
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
            DEBUGH('t', "Completing entry for prototype "
                    << *(function_id) << endl);
            size_t sequence_nr = 0;
            status = validate_block(function->third(), function_id,
                    sequence_nr);
            if(status != 0) return status;
            prototype->has_block = true;
        }
    } else { 
        globals->insert({function_id, function_entry});
        if(function->children.size() == 3) {
            DEBUGH('t', "No protoype; defining function "
                    << *(function_id) << endl);
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
        DEBUGH('t', "Making entry for local var " <<
                identifier->lexinfo);
        identifier->attributes.set((size_t)attr::LVAL);
        identifier->attributes.set((size_t)attr::VARIABLE);
        int status = validate_type_id(local);
        if(status != 0) return status;
        symbol_value* local_value = new struct 
                symbol_value(local->second(), sequence_nr, next_block);
        locals->insert({identifier->lexinfo, local_value});
        ++sequence_nr;
        return 0;
    }
}

int type_checker::validate_block(astree* block,
        const string* function_name, size_t& sequence_nr) {
    for(astree* statement : block->children) {
        int status = validate_stmt_expr(statement, function_name,
                sequence_nr);
        if (status != 0) return status;
    }
    return 0;
}

int type_checker::validate_stmt_expr(astree* statement,
        const string* function_name, size_t& sequence_nr) {
    int status;
    const string* ident;
    symbol_value* function = function_name == &DUMMY_FUNCTION ?
            nullptr : globals->at(function_name);
    DEBUGH('t', "Validating next statement/expression");
    switch(statement->symbol) {
        // trees generated my the "statement" production
        case TOK_RETURN:
            if(statement->children.empty()) {
                if(!function->attributes.test(
                        (size_t)attr::VOID)) {
                    cerr << "ERROR: Return statement with value in"
                         << " void function: " << *function_name
                         << endl;
                    return -1;
                }
            } else {
                validate_stmt_expr(statement->first(), function_name,
                        sequence_nr);
                status = types_equal(statement->first(), function);
                if(status == 0) {
                    cerr << "ERROR: Incompatible return type" << endl;

                    return status;
                }
            }  
            break;
        case TOK_TYPE_ID:
            statement->first()->attributes.set((size_t)attr::LOCAL);
            status = make_local_entry(statement, sequence_nr);
            if(status != 0) return status;
            break;
        case TOK_IF:
            status = validate_stmt_expr(statement->first(),
                    function_name, sequence_nr);
            if(status != 0) return status;
            status = validate_stmt_expr(statement->second(),
                    function_name, sequence_nr);
            if(status != 0) return status;
            if(statement->children.size() == 3)
            status = validate_stmt_expr(statement->third(),
                    function_name, sequence_nr);
            if(status != 0) return status;
        break;
        case TOK_WHILE:
            status = validate_stmt_expr(statement->first(),
                    function_name, sequence_nr);
            if(status != 0) return status;
            status = validate_stmt_expr(statement->second(),
                    function_name, sequence_nr);
            if(status != 0) return status;
            break;
        case TOK_BLOCK:
            status = validate_block(statement, function_name,
                    sequence_nr);
            if(status != 0) return status;
            break;
        // gray area; vardecls (a statement) and assignments (an
        // expression) can both have an '=' as their root but we handle
        // them the same way
        case '=':
            status = validate_stmt_expr(statement->first(),
                    function_name, sequence_nr);
            if(status != 0) return status;
            status = validate_stmt_expr(statement->second(),
                    function_name, sequence_nr);
            if(status != 0) return status;
            if(!types_equal(statement->first(), statement->second())) {
                cerr << "ERROR: Incompatible types for tokens: "
                     << parser::get_tname(statement->first()->symbol)
                     << " " << parser::get_tname(statement->second()->
                        symbol) << endl;
                return -1;
            } else if(!statement->first()->attributes.test(
                    (size_t)attr::LVAL)) {
                cerr << "ERROR: Destination is not an LVAL" << endl;
                return -1;
            }
            // type is the type of the left operand
            DEBUGH('t', "Setting return attributes to "
                    << statement->first()->attributes);
            statement->attributes = statement->first()->attributes;
            break;
        // here begins the trees made by the "expr" production
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
            status = validate_stmt_expr(statement->first(),
                    function_name, sequence_nr);
            if(status != 0) return status;
            status = validate_stmt_expr(statement->second(),
                    function_name, sequence_nr);
            if(status != 0) return status;
            if(!types_equal(statement->first(), statement->second())) {
                cerr << "ERROR: Incompatible types for operator: "
                     << statement->symbol << endl;
                return -1;
            } else if(!statement->first()->attributes.test(
                (size_t)attr::INT)) {
                cerr << "ERROR: Operator " << (char)statement->symbol
                     << " must have operands of type int" << endl;
                return -1;
            }
            break;
        case TOK_NOT:
        case TOK_POS:
        case TOK_NEG:
            // unary operators
            status = validate_stmt_expr(statement->first(),
                    function_name, sequence_nr);
            if(status != 0) return status;
            if(!statement->first()->attributes.test((size_t)attr::INT)) {
                cerr << "ERROR: '" << *(statement->lexinfo)
                     << "' argument must be of type int" << endl;
                return -1;
            }
            break;
        case TOK_ALLOC:
            status = validate_type_id(statement->first(), statement);
            if(status != 0) return status;
            break;
        case TOK_CALL:
            statement->attributes.set((size_t)attr::VREG);
            status = validate_call(statement);
            if(status != 0) return status;
            break;
        case TOK_INDEX:
            // evaluate left and right side
            // make sure left is an array and right is an int
            // set type to type of array, minus the array
            status = validate_stmt_expr(statement->first(),
                    function_name, sequence_nr);
            if(status != 0) return status;
            status = validate_stmt_expr(statement->second(),
                    function_name, sequence_nr);
            if(status != 0) return status;
            if(!(statement->first()->attributes.test((size_t)attr::
                    ARRAY) && statement->second()->attributes.test(
                    (size_t)attr::INT))) {
                cerr << "ERROR: Incompatible types for indexing"
                     << " operator" << endl;
                return -1;
            }
            statement->attributes |= statement->first()->attributes;
            statement->attributes.reset((size_t)attr::ARRAY);
            break;
        case TOK_ARROW:
            // evaluate left but not right since right
            // is always an ident
            status = validate_stmt_expr(statement->first(),
                    function_name, sequence_nr);
            if(status != 0) return status;
            if(statement->first()->attributes.test((size_t)attr::
                    STRUCT) && type_names->find(
                    statement->first()->type_id) != type_names->end()) {
                // get struct def and check to make sure the field
                // is in it,
                symbol_value* struct_def = type_names->at(statement->
                        first()->type_id);
                if(struct_def->fields->find(statement->
                        second()->lexinfo) != struct_def->fields->end()) {
                    symbol_value* field_def = struct_def->fields->
                            at(statement->second()->lexinfo);
                    assign_type(statement->second(), field_def);
                    assign_type(statement, field_def);
                } else {
                    cerr << "ERROR: field "
                         << *(statement->second()->lexinfo)
                         << " is not a member of structure"
                         << endl;
                    return -1;
                }
            } else {
                cerr << "ERROR: structure " << *(statement->first()->type_id)
                     << " not defined for token "
                     << *(statement->first()->lexinfo) << endl;
                return -1;
            }
            break;
        case TOK_INTCON:
        case TOK_NULLPTR:
        case TOK_CHARCON:
            // types are set on construction and we don't need to do
            // anything else
            break;
        case TOK_STRINGCON:
            // save for intlang
            string_constants.push_back(*(statement->lexinfo));
            break;
        case TOK_IDENT:
            status = assign_type(statement);
            if(status != 0) return status;
            break;
        default:
            cerr << "ERROR: UNEXPECTED TOKEN IN STATEMENT/EXPRESSION: "
                 << *(statement->lexinfo) << endl;
            return -1;
    }
    return 0;
}

int type_checker::validate_type_id(astree* type_id) {
    return validate_type_id(type_id->first(), type_id->second());
}

int type_checker::validate_type_id(astree* type, astree* identifier) {
    DEBUGH('t', "Validating typeid of symbol " <<
            *(identifier->lexinfo));
    if(type->symbol == TOK_ARRAY) {
        DEBUGH('t', "Setting attribute ARRAY");
        identifier->attributes.set((size_t)attr::ARRAY);
        type = type->first();
    }
    switch(type->symbol) {
        case TOK_INT:
            DEBUGH('t', "Setting attribute INT");
            identifier->attributes.set((size_t)attr::INT);
            break;
        case TOK_STRING:
            DEBUGH('t', "Setting attribute STRING");
            identifier->attributes.set((size_t)attr::STRING);
            break;
        case TOK_PTR:
            DEBUGH('t', "Setting attribute STRUCT");
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
            DEBUGH('t', "Setting attribute VOID");
            if(identifier->attributes.test((size_t)attr::ARRAY)) {
                cerr << "ERROR: you may not have arrays of type void!"
                     << endl;
                return -1;
            } else if(!identifier->attributes.test((size_t)attr::
                    FUNCTION)) {
                cerr << "ERROR: variables and fields cannot be of type "
                     << "void!" << endl;
                return -1;
            } else {
                identifier->attributes.set((size_t)attr::VOID);
            }
            break;
    }
    DEBUGH('t', "id now has attributes: " << identifier->attributes);
    return 0;
}

int type_checker::validate_call(astree* call) {
    const string* identifier = call->first()->lexinfo;
    // params are at the same level as the id
    vector<astree*> params = call->children;
    if(globals->find(identifier) != globals->end()) {
        symbol_value* function = globals->at(identifier);
        if(params.size()-1 != function->parameters.size()) {
            cerr << "ERROR: incorrect number of arugments: "
                 << *identifier << endl;
            return -1;
        }
        DEBUGH('t', "Validating arguments for call to "
                << *identifier << " num params: " << params.size()-1);
        for(size_t i = 0; i < function->parameters.size(); ++i) {
        DEBUGH('t', "Validating argument " << i);
            astree* param = params[i+1];
            size_t dummy_sequence = 0;
            DEBUGH('t', "Argument was not null");
            int status = validate_stmt_expr(param,
                    &DUMMY_FUNCTION, dummy_sequence);
            if(status != 0) return status;
            DEBUGH('t', "Comparing types");
            if(!types_equal(param, function->parameters[i])) {
                cerr << "ERROR: incompatible type for argument: "
                     << *(param->lexinfo) << endl;
                return -1;
            }
        }
        call->attributes |= function->attributes;
        call->first()->attributes |= function->attributes;
        return 0;
    } else {
        cerr << "ERROR: Invalid call to function: "
             << *identifier << endl;
        return -1;
    }
}

int type_checker::assign_type(astree* ident) {
    DEBUGH('t', "Attempting to assign a type");
    const string* id_str = ident->lexinfo;
    if(locals->find(id_str) != locals->end()) {
        DEBUGH('t', "Assigned " << *id_str << " a local value");
        ident->attributes = locals->at(id_str)->attributes;
        ident->type_id = locals->at(id_str)->type_id;
    } else if(globals->find(id_str) != globals->end()) {
        DEBUGH('t', "Assigned " << *id_str << " a global value");
        ident->attributes = globals->at(id_str)->attributes;
        ident->type_id = globals->at(id_str)->type_id;
    } else {
        cerr << "ERROR: could not resolve symbol: "
             << (ident->lexinfo) << " "
             << parser::get_tname(ident->symbol) << endl;
        return -1;
    }
    return 0;
}

int type_checker::assign_type(astree* ident, symbol_value* value) {
    DEBUGH('t', "Type has been found; assigning");
    ident->attributes |= value->attributes;
    return 0;
}

int type_checker::assign_type(astree* dest, astree* source) {
    dest->attributes |= source->attributes;
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
    if(t1->symbol == TOK_TYPE_ID) t1 = t1->second();
    return types_equal(t1->attributes, t2->attributes);
}

bool type_checker::types_equal(astree* tree, symbol_value* entry) {
    if(tree->symbol == TOK_TYPE_ID) tree = tree->second();
    return types_equal(tree->attributes, entry->attributes);
}

bool type_checker::types_equal(attr_bitset a1, attr_bitset a2) {
    DEBUGH('t', "Comparing bitsets: " << a1 << " and " << a2);
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
        DEBUGH('t', "OK: Both types int");
        return true;
    } else {
        DEBUGH('t', "OOF");
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
