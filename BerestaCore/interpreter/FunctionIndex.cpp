//
// Created by Denis on 14.10.2025.
//

#include "interpreter/FunctionIndex.h"
#include "frontend/parser/Statement.h"

void FunctionIndex::reindex_file(const std::string& file, const std::vector<std::unique_ptr<Statement>>& ast)
{
    auto& priv = _private_by_file[file];
    priv.clear();

    for(const auto& st : ast)
    {
        if(!st || st->type != StatementType::FUNCTION) {continue;}

        auto* fn = dynamic_cast<FunctionStatement*>(st.get());
        if(!fn) {continue;}

        if(fn->visibility == FunctionVisibility::PUBLIC) {_public[fn->name] = {fn, file, true };}
        else                                             {priv[fn->name] = fn;}
    }
}

FunctionRef* FunctionIndex::find_public(const std::string& name)
{
    auto it = _public.find(name);
    return (it == _public.end()) ? nullptr : &it->second;
}

FunctionStatement* FunctionIndex::find_private_in_file(const std::string& file, const std::string& name)
{
    auto itf = _private_by_file.find(file);
    if(itf == _private_by_file.end()) {return nullptr;}
    auto it = itf->second.find(name);
    return (it == itf->second.end()) ? nullptr : it->second;
}

const FunctionRef* FunctionIndex::find_function(const std::string& name, const std::string& current_file)
{
    if(auto* fn = find_private_in_file(current_file, name))
    {
        auto& ref = _private_cache[current_file][name];
        ref.func = fn;
        ref.file = current_file;
        ref.is_public = false;
        return &ref;
    }
    return find_public(name);
}
