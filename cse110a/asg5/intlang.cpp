int generator::write_var_decl(astree* var) {
    astree* id = var->second();
    if(id->has_attr(attr::LOCAL)) {
        out << ".local " << write_type(id) << *(id->lexinfo);
    } else if(id->has_attr(attr::STRING)) {
        // strings are anonymous
        out << ".string .s" << /*string count++*/ << " \"";
    } else if(id->has_attr(attr::FIELD)) {
        out << TAB << ".field " << write_type(id) << *(id->lexinfo);
    } else {
        out << ".global " << write_type(id) << *(id->lexinfo);
    }
    return 0;
}

int generator::write_struct_decl(astree* structure ) {
    astree* id = structure->second();
    out << ".struct " << *(id->lexinfo) << endl;
    if(structure->chlidren.size() == 3) {
        vector<astree*> fields = id->third()->children;
        for(const auto& iter = fields.begin(); iter != fields.end();
                ++iter) {
            out << TAB << write_type(*iter->second())
                << *(*iter->second->lexinfo) << endl;
        }
    }
    out << ".end" << endl;
}

int generator::write_function_decl(astree* fun) {
    // write function header (type, name, args, etc)
    astree* fun_name_node = fun->first()->second();
    out << ".function " << write_type(fun_name_node, out)
        << *(fun_name_node->lexinfo) << " (";
    vector<astree*> params = fun->second->children;
    for(size_t i = 0; i < params.size(); ++i) {
        if(i > 0) out << ", ";
        param_name_node = params[i]->second();
        out << write_type(param_name_node, out)
            << *(param_name_node->lexinfo)
    }
    out << ")" << endl;
    // write the block
    out << TAB << "return" << endl;
    out << ".end" << endl;
}

int generator::write_alloc(astree* alloc, size_t branch, size_t reg) {
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
}

ofstream& generator::write_type(astree* tree, ofstream& out) {
    if(tree->has_attr(attr::INT)) {
        out << attr::INT << " ";
    } else if(tree->has_attr(attr::VOID)) {
        out << " ";
    } else if(tree->has_attr(attr::STRUCT)) {
        out << "void* ";
    } else if(tree->has_attr(attr::STRING)) {
        out << "void* ";
    }
    return out;
}
