#include <utility>
#include <string>

#include "astree.h"
#include "lyutils.h"
#include "symtable.h"
#include "auxlib.h"
#include "intlang.h"

using symbol_pair = pair<const string*, symbol_value*>;
size_t generator::string_count = 0;
size_t generator::branch_count = 0;

int generator::write_int_lang(astree* root, symbol_table* structures,
        symbol_table* globals, vector<symbol_table*> locals,
        vector<string> strings) {
    vector<symbol_pair> sorted_structures =
            type_checker::sort_symtable(structures);
    for(const auto& itor = sorted_structures.begin();
            itor != sorted_structures.end(); ++itor) {
        write_struct_decl(*itor);
    }
    for(const auto& itor = strings.begin(); itor != strings.end();
            ++itor) {
        write_string_decl(*itor);
    }
    vector<symbol_pair> sorted_globals =
            type_checker::sort_symtable(globals);
    // probably unnecessary but since we're going over globals
    vector<symbol_pair> sorted_functions;
    for(const auto& itor = sorted_globals.begin();
            itor != sorted_globals.end(); ++itor) {
        if(itor->second->has_attr(attr::FUNCTION))
            sorted_functions.push_back(*itor);
        else
            write_var_decl(*itor);
    }

    vector<astree*> statements = root->children;
    for(const auto& itor = statements->cbegin();
            itor != statements.cend(); ++itor)) {
        if(*itor->has_attr(attr::FUNCTION)) {
            size_t fn_block = *itor->second->block_nr - 1;
            write_function_decl(*itor, locals[fn_block]);
        }
    }
}

int generator::write_var_decl(symbol_pair pair) {
    const string* id = pair.first;
    symbol_value* value = pair.second;
    if(value->has_attr(attr::LOCAL)) {
        out << TAB << ".local " << write_type(value) << *(id) << endl;
    } else if(value->has_attr(attr::FIELD)) {
        out << TAB << ".field " << write_type(value) << *(id) << endl;
    } else {
        out << ".global " << write_type(value) << *(id) << endl;
    }
    return 0;
}

int generator::write_string_decl(string s) {
    out << ".string .s" << string_count++ << " \"" << s << "\"";
}

int generator::write_struct_decl(symbol_pair pair) {
    const string* id = pair.first;
    symbol_value* value = pair.second;
    symbol_table* fields = value->fields;
    out << ".struct " << *(id) << endl;
    for(size_t bucket = 0; bucket < fields->bucket_count(); ++bucket) {
        for(auto itor = fields->cbegin(bucket);
                itor != fields->cend(bucket); ++itor) {
            write_var_decl(*itor);
        }
    }
    out << ".end" << endl;
}

int generator::write_function_decl(astree* fun, symbol_table* locals) {
    // write function header (type, name, args, etc)
    astree* fun_name_node = fun->first()->second();
    out << ".function " << write_type(fun_name_node, out)
        << *(fun_name_node->lexinfo) << " (";
    vector<astree*> params = fun->second->children;
    for(size_t i = 0; i < params.size(); ++i) {
        if(i > 0) out << ", ";
        param_name_node = params[i]->second();
        out << write_type(param_name_node, out)
            << *(param_name_node->lexinfo);
    }
    out << ")" << endl;
    // write the block
    vector<symbol_pair> sorted_locals =
            type_checker::sort_symtable(locals);
    for(const auto& itor = sorted_locals.cbegin(); itor !=
            sorted_locals.cend(); ++itor) {
        write_var_decl(*itor);
    }
    out << TAB << "return" << endl;
    out << ".end" << endl;
}

/*int generator::write_alloc(astree* alloc, size_t branch, size_t reg) {
    if(alloc->first()->has_attr(attr::STRUCT)) {
        const string* struct_id = alloc->first()->lexinfo;
        out << TAB << "malloc (sizeof struct " << *struct_id
            << ")" << endl;
    } else if(alloc->first()->has_attr(attr::STRING)) {
        // evaluate the expression
        // if a vreg was created, use the vreg
        out << TAB << "malloc (" << "yeet" << ")" << endl;
    } else if(alloc->first()->has_attr(attr::ARRAY)) {
        // evaluate the expression
        // determine plaintype
        // if a vreg was created, use the vreg
        string plaintype_size = alloc->first()->first()->
                has_attr(attr::STRUCT) ? "ptr" : "int";
        // put sizeof(x) * n into a vreg
        out << TAB << "malloc (" << "vreg goes here" << ")" << endl;
    } else {
        // error
    }
    return 0;
}*/

ofstream& generator::write_type(astree* tree, ofstream& out) {
    if(tree->has_attr(attr::ARRAY)) {
        out << "void* ";
    } else if(tree->has_attr(attr::INT)) {
        out << "int ";
    } else if(tree->has_attr(attr::VOID)) {
        out << " ";
    } else if(tree->has_attr(attr::STRUCT)) {
        out << "struct " << tree->type_id << " ";
    } else if(tree->has_attr(attr::STRING)) {
        out << "void* ";
    }
    return out;
}

ofstream& generator::write_type(symbol_value* value, ofstream& out) {
    // i think this is the type for all arrays?
    if(value->has_attr(attr::ARRAY)) {
        out << "void* ";
    } else if(value->has_attr(attr::INT)) {
        out << "int ";
    } else if(value->has_attr(attr::VOID)) {
        out << " ";
    } else if(value->has_attr(attr::STRUCT)) {
        out << "struct " << value->type_id << " ";
    } else if(value->has_attr(attr::STRING)) {
        out << "void* ";
    }
    return out;
}
