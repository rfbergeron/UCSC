#include "symtable.h"

int type_checker::make_symbol_table(astree* root) {
    int ret = 0;
    for(const astree* child : root->children) {
        const string symbol_type = {parser::get_tname(child->symbol)};
        if(symbol_type.compare("TOK_STRUCT") {
            // handle structure definition
        } else if(symbol_type.compare("TOK_FUNCTION") {
            // handle function definition
        } else if(symbol_type.compare("TOK_TYPE_ID") {
            // handle global identifier declaration
        }
    }
    return ret;
}

void type_checker::set_attributes(symbol* entry, const string token) {

}

int type_checker::make_function_entry(astree* function) {
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
}

int type_checker::make_structure_entry(astree* structure) {
    const string structure_type_name = structure->children[
}

int type_checker::make_global_entry(astree* global) {
}

void type_checker::dump_symbol_table() {
}
