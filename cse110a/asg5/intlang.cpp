#include <utility>
#include <string>
#include <vector>
#include <unordered_map>

#include "astree.h"
#include "lyutils.h"
#include "symtable.h"
#include "auxlib.h"
#include "intlang.h"

using symbol_table = unordered_map<const string*,symbol_value*>;
using symbol_entry = symbol_table::value_type;

size_t generator::branch_count = 0;
size_t generator::vreg_count = 0;
ostream* generator::out = &cout;
const string generator::TAB = "          ";

void generator::set_out(ostream* out_) {
    out = out_;
}

int generator::write_int_lang(astree* root,
        vector<symbol_table*> tables,
        vector<string> strings) {
    DEBUGH('l', "Writing structure definitions");
    vector<symbol_entry> sorted_structures =
            type_checker::sort_symtable(tables[tables.size()-1]);
    tables.pop_back();
    for(auto&& itor = sorted_structures.cbegin();
            itor != sorted_structures.end(); ++itor) {
        write_struct_decl(*itor);
    }

    DEBUGH('l', "Writing string constant definitions");
    for(size_t i = 0; i < strings.size(); ++i) {
        write_string_decl(strings[i], i);
    }
    *out << endl;

    DEBUGH('l', "Writing global definitions");
    vector<symbol_entry> sorted_globals =
            type_checker::sort_symtable(tables[tables.size()-1]);
    tables.pop_back();
    // probably unnecessary but since we're going over globals
    vector<symbol_entry> sorted_functions;
    for(auto&& itor = sorted_globals.cbegin();
            itor != sorted_globals.end(); ++itor) {
        if(itor->second->has_attr(attr::FUNCTION))
            sorted_functions.push_back(*itor);
        else
            write_var_decl(*itor);
    }
    *out << endl;

    vector<astree*> statements = root->children;
    for(auto&& itor = statements.cbegin();
            itor != statements.cend(); ++itor) {
        if((*itor)->symbol == TOK_FUNCTION) {
            DEBUGH('l', "Writing a function definition");
            size_t fn_block = (*itor)->second()->block_nr - 1;
            write_function_decl(*itor, tables[fn_block]);
        }
    }
    return 0;
}

int generator::write_var_decl(symbol_entry pair) {
    const string* id = pair.first;
    symbol_value* value = pair.second;
    if(value->has_attr(attr::LOCAL)) {
        *out << TAB << ".local ";
        write_type(value);
        *out << *(id) << endl;
    } else if(value->has_attr(attr::FIELD)) {
        *out << TAB << ".field ";
        write_type(value);
        *out << *(id) << endl;
    } else {
        *out << ".global ";
        write_type(value);
        *out << *(id) << endl;
    }
    return 0;
}

int generator::write_string_decl(string s, size_t strcount) {
    *out << ".string .s" << strcount << " " << s << endl;
    return 0;
}

int generator::write_struct_decl(symbol_entry pair) {
    const string* id = pair.first;
    symbol_value* value = pair.second;
    symbol_table* fields = value->fields;
    *out << ".struct " << *(id) << endl;
    for(size_t bucket = 0; bucket < fields->bucket_count(); ++bucket) {
        for(auto&& itor = fields->cbegin(bucket);
                itor != fields->cend(bucket); ++itor) {
            write_var_decl(*itor);
        }
    }
    *out << ".end" << endl << endl;
    return 0;
}

int generator::write_function_decl(astree* fun, symbol_table* locals) {
    // write function header (type, name, args, etc)
    astree* fun_name_node = fun->first()->second();
    *out << ".function ";
    write_type(fun_name_node);
    *out << *(fun_name_node->lexinfo) << " (";
    vector<astree*> params = fun->second()->children;
    for(size_t i = 0; i < params.size(); ++i) {
        if(i > 0) *out << ", ";
        astree* param_name_node = params[i]->second();
        write_type(param_name_node);
        *out << *(param_name_node->lexinfo);
    }
    *out << ")" << endl;

    // write local declarations
    DEBUGH('l', "Emitting local declarations");
    vector<symbol_entry> sorted_locals =
            type_checker::sort_symtable(locals);
    for(auto&& itor = sorted_locals.cbegin(); itor !=
            sorted_locals.cend(); ++itor) {
        if(!((*itor).second->has_attr(attr::PARAM))) {
            write_var_decl(*itor);
        } else {
        }
    }

    // write block
    if(fun->children.size() == 3) {
        branch_count = 9;
        vreg_count = 0;
        vector<astree*> block_statements = fun->third()->children;
        DEBUGH('l', "Writing block");
        for(auto && itor = block_statements.begin();
                itor != block_statements.end(); ++itor) {
            write_stmt_expr(*itor);
        }
    }

    // terminate function declaration
    *out << TAB << "return" << endl;
    *out << ".end" << endl << endl;
    return 0;
}

void generator::write_stmt_expr(astree* expr) {
    DEBUGH('l', "Writing expression");
    size_t left_regid;
    size_t right_regid;
    switch(expr->symbol) {
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
            DEBUGH('l', "Expression is integer operator");
            write_stmt_expr(expr->first());
            left_regid = vreg_count;
            write_stmt_expr(expr->second());
            right_regid = vreg_count;
            ++vreg_count;

            *out << "$t" << vreg_count << ":i = ";
            if(expr->first()->symbol == TOK_IDENT) {
                *out << *(expr->first()->lexinfo);
            } else {
                *out << "$t" << left_regid << ":i";
            }

            *out << " " << expr->symbol << " ";

            if(expr->second()->symbol == TOK_IDENT) {
                *out << *(expr->second()->lexinfo);
            } else {
                *out << "$t" << right_regid << ":i";
            }
            *out << endl;
            break;
        case '=':
            DEBUGH('l', "Expression is assignment");
            write_stmt_expr(expr->first());
            left_regid = vreg_count;
            write_stmt_expr(expr->second());
            right_regid = vreg_count;

            if(expr->first()->symbol = TOK_IDENT) {
                *out << *(expr->first()->lexinfo);
            } else if(expr->first()->symbol = TOK_TYPE_ID) {
                *out << *(expr->first()->second()->lexinfo);
            } else if(expr->first()->has_attr(attr::VADDR)) {
                *out << "$t" << left_regid << "X";
            } else {
                *out << "GOOFY MF";
            }
            break;
        case TOK_TYPE_ID:
            DEBUGH('l', "Expression is declaration");
            if(expr->children.size() == 3) {
                DEBUGH('l', "Assignment occurs at declaration");

            }
            break;
        default:
            cerr << "UNIMPLEMENTED: " << *(expr->lexinfo) << endl;
    }
}

void generator::write_type(astree* tree) {
    // i think this is the type for all arrays?
    if(tree->has_attr(attr::ARRAY)) {
        *out << "void* ";
    } else if(tree->has_attr(attr::INT)) {
        *out << "int ";
    } else if(tree->has_attr(attr::VOID)) {
        *out << " ";
    } else if(tree->has_attr(attr::STRUCT)) {
        *out << "struct " << *(tree->type_id) << " ";
    } else if(tree->has_attr(attr::STRING)) {
        *out << "void* ";
    }
}

void generator::write_type(symbol_value* value) {
    // i think this is the type for all arrays?
    if(value->has_attr(attr::ARRAY)) {
        *out << "void* ";
    } else if(value->has_attr(attr::INT)) {
        *out << "int ";
    } else if(value->has_attr(attr::VOID)) {
        *out << " ";
    } else if(value->has_attr(attr::STRUCT)) {
        *out << "struct " << *(value->type_id) << " ";
    } else if(value->has_attr(attr::STRING)) {
        *out << "void* ";
    }
}
