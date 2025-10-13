//
// Created by Denis on 09.10.2025.
//

#include "BuiltinDictionary.h"
#include "builtin/core/BuiltinRegistry.h"
#include "builtin/core/BuiltinUtils.h"
#include "diagnostics/Diagnostics.h"
#include <iostream>

static bool ensure_dict_arg(Diagnostics& diag, const std::string& file, int line, const std::vector<Value>& args, size_t idx, const std::string& name)
{
    if(idx >= args.size()) {diag.error(name + ": missing argument #" + std::to_string(idx), file, line); return false;}
    if(args[idx].type != ValueType::DICTIONARY) {diag.error(name + ": argument #" + std::to_string(idx) + " must be dictionary", file, line); return false;}
    return true;
}

Value BuiltinDictionaryKeys::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 1, 1);
    if(!ensure_dict_arg(diag, file, line, args, 0, name())) {return {};}
    const auto& dict = *std::get<DictionaryPtr>(args[0].data);
    std::vector<Value> keys;
    keys.reserve(dict.size());
    for(const auto& [k, v] : dict)
    {
        keys.emplace_back(k);
    }
    return Value(keys);
}

Value BuiltinDictionaryValues::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 1, 1);
    if(!ensure_dict_arg(diag, file, line, args, 0, name())) {return {};}
    const auto& dict = *std::get<DictionaryPtr>(args[0].data);
    std::vector<Value> values;
    values.reserve(dict.size());
    for(const auto& [k, v] : dict)
    {
        values.push_back(v);
    }
    return Value(values);
}

Value BuiltinDictionaryHas::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 2, 2);
    if(!ensure_dict_arg(diag, file, line, args, 0, name())) {return {};}
    if(args[1].type != ValueType::STRING) {diag.error("dictionary_has: key must be string", file, line); return {};}
    const auto& dict = *std::get<DictionaryPtr>(args[0].data);
    const auto& key = std::get<std::string>(args[1].data);
    return Value(dict.find(key) != dict.end());
}

Value BuiltinDictionaryGet::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_min_arity(diag, file, line, args, 2);
    if(!ensure_dict_arg(diag, file, line, args, 0, name())) {return {};}
    if(args[1].type != ValueType::STRING) {diag.error("dictionary_get: key must be string", file, line); return {};}
    const auto& dict = *std::get<DictionaryPtr>(args[0].data);
    const auto& key = std::get<std::string>(args[1].data);
    auto it = dict.find(key);
    if(it != dict.end()) {return it->second;}
    if(args.size() == 3) {return args[2];}
    return {};
}

Value BuiltinDictionarySet::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 3, 3);
    if(!ensure_dict_arg(diag, file, line, args, 0, name())) {return {};}
    if(args[1].type != ValueType::STRING) {diag.error("dictionary_set: key must be string", file, line); return {};}
    auto& dict = *std::get<DictionaryPtr>(args[0].data);
    const auto& key = std::get<std::string>(args[1].data);
    dict[key] = args[2];
    return args[0];
}

Value BuiltinDictionaryDelete::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 2, 2);
    if(!ensure_dict_arg(diag, file, line, args, 0, name())) {return {};}
    if(args[1].type != ValueType::STRING) {diag.error("dictionary_delete: key must be string", file, line); return {};}
    auto& dict = *std::get<DictionaryPtr>(args[0].data);
    const auto& key = std::get<std::string>(args[1].data);
    dict.erase(key);
    return args[0];
}

Value BuiltinDictionarySize::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 1, 1);
    if(!ensure_dict_arg(diag, file, line, args, 0, name())) {return {};}
    const auto& dict = *std::get<DictionaryPtr>(args[0].data);
    return Value(static_cast<int>(dict.size()));
}

Value BuiltinDictionaryClear::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 1, 1);
    if(!ensure_dict_arg(diag, file, line, args, 0, name())) {return {};}
    auto& dict = *std::get<DictionaryPtr>(args[0].data);
    dict.clear();
    return args[0];
}

Value BuiltinDictionaryCopy::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 1, 1);
    if(!ensure_dict_arg(diag, file, line, args, 0, name())) {return {};}
    const auto& dict = *std::get<DictionaryPtr>(args[0].data);
    return Value(Dictionary(dict));
}

Value BuiltinDictionaryMerge::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 2, 2);
    if(!ensure_dict_arg(diag, file, line, args, 0, name())) {return {};}
    if(!ensure_dict_arg(diag, file, line, args, 1, name())) {return {};}
    auto& dict1 = *std::get<DictionaryPtr>(args[0].data);
    const auto& dict2 = *std::get<DictionaryPtr>(args[1].data);
    for(const auto& [k, v] : dict2)
    {
        dict1[k] = v;
    }
    return args[0];
}

Value BuiltinDictionaryDestroy::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 1, 1);
    if(!ensure_dict_arg(diag, file, line, args, 0, name())) {return {};}
    const_cast<Value&>(args[0]) = Value();
    return {};
}

void register_builtin_dictionary()
{
    auto& reg = BuiltinRegistry::instance();
    reg.register_builtin(std::make_unique<BuiltinDictionaryKeys>());
    reg.register_builtin(std::make_unique<BuiltinDictionaryValues>());
    reg.register_builtin(std::make_unique<BuiltinDictionaryHas>());
    reg.register_builtin(std::make_unique<BuiltinDictionaryGet>());
    reg.register_builtin(std::make_unique<BuiltinDictionarySet>());
    reg.register_builtin(std::make_unique<BuiltinDictionaryDelete>());
    reg.register_builtin(std::make_unique<BuiltinDictionarySize>());
    reg.register_builtin(std::make_unique<BuiltinDictionaryClear>());
    reg.register_builtin(std::make_unique<BuiltinDictionaryCopy>());
    reg.register_builtin(std::make_unique<BuiltinDictionaryMerge>());
    reg.register_builtin(std::make_unique<BuiltinDictionaryDestroy>());
}
