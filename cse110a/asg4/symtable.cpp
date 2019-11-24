#include "astree.h"
#include "lyutils.h"
#include "symtable.h"

using symbol_table = unordered_map<const string*,symbol_value*>;
symbol_table* type_checker::type_names = new symbol_table();
symbol_table* type_checker::globals = new symbol_table();
symbol_table* type_checker::locals = new symbol_table();

int type_checker::make_symbol_table(astree* root) {
    int ret = 0;
    for(astree* child : root->children) {
        const string symbol_type = {parser::get_tname(child->symbol)};
        if(symbol_type.compare("TOK_STRUCT")) {
            // handle structure definition
        } else if(symbol_type.compare("TOK_FUNCTION")) {
            // handle function definition
        } else if(symbol_type.compare("TOK_TYPE_ID")) {
            // handle global identifier declaration
        } else if(symbol_type.compare("'='")) {
            // global identifier, but there is an expression
            // whose type we need to verify
        }
    }
    return ret;
}

int type_checker::make_global_entry(astree* global) {
    if(globals->find(global->lexinfo) != globals->end()) {
        //error; duplicate declaration
        return -1;
    } else {
        symbol_value* global_value = new symbol_value();
        global_value->attributes |= global->attributes;
        globals->insert({global->lexinfo, global_value});
        return 0;
    }
}

int type_checker::make_structure_entry(astree* structure) {
    const string* structure_type = structure->children[0]->lexinfo;
    int ret = 0;
    if(type_names->find(structure_type) != type_names->end()) {
        // error; duplicate declaration
        ret = -1;
    } else {
        symbol_value* structure_value = new symbol_value();
        structure_value->fields = new symbol_table();
        structure_value->attributes |= structure->attributes;
        for(size_t i = 1; i < structure->children.size(); ++i) {
            astree* field_node = structure->children[i];
            if(structure_value->fields->find(field_node->lexinfo) !=
                    structure_value->fields->end()) {
                // error; duplicate field names
                ret = -1;
            } else {
                symbol_value* field_value = new symbol_value();
                // maybe need to set sequence # for fields
                field_value->attributes |= field_node->attributes;
                structure_value->fields->insert({field_node->lexinfo,
                        field_value});
            }
        }
        type_names->insert({structure_type, structure_value});
    }
    return ret;
}

/*int type_checker::make_function_entry(astree* function) {
    int ret = 0;
    struct symbol* function_entry = new struct symbol();
    function_entry->lloc = function->loc;

    // figure out return type
    string function_type_tok =
            {parser::get_tname(function->children[0]->symbol)};
    if(function_type_tok.compare("TOK_PTR") {
        symbol* structure_def =
                type_names[function->children[0]->children[0]->lexinfo];
        if(structure_def == nullptr) {
            // error; structure not defined
        }
    } else {
        
    }
    // check and add parameters
    for(const astree* parameter_node : function->children[1]->children) {
        astree* type_node = parameter_node->children[1];
        string type_tok = {parser::get_tname(type_node->symbol)};
        if(type_tok.compare("TOK_ARRAY") {
            // handle array type
        }
    }
}*/

void type_checker::destroy_tables() {
    delete type_names;
    delete globals;
    delete locals;
}
