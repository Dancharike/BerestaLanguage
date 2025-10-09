//
// Created by Denis on 09.10.2025.
//

#ifndef BERESTALANGUAGE_BUILTINDICTIONARY_H
#define BERESTALANGUAGE_BUILTINDICTIONARY_H

#pragma once
#include "builtin/core/IBuiltinFunction.h"

struct BuiltinDictionaryKeys : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "dictionary_keys";}
    Value invoke(const std::vector<Value>& args) override;
};

struct BuiltinDictionaryValues : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "dictionary_values";}
    Value invoke(const std::vector<Value>& args) override;
};

struct BuiltinDictionaryHas : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "dictionary_has";}
    Value invoke(const std::vector<Value>& args) override;
};

struct BuiltinDictionaryGet : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "dictionary_get";}
    Value invoke(const std::vector<Value>& args) override;
};

struct BuiltinDictionarySet : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "dictionary_set";}
    Value invoke(const std::vector<Value>& args) override;
};

struct BuiltinDictionaryDelete : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "dictionary_delete";}
    Value invoke(const std::vector<Value>& args) override;
};

struct BuiltinDictionarySize : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "dictionary_size";}
    Value invoke(const std::vector<Value>& args) override;
};

struct BuiltinDictionaryClear : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "dictionary_clear";}
    Value invoke(const std::vector<Value>& args) override;
};

struct BuiltinDictionaryCopy : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "dictionary_copy";}
    Value invoke(const std::vector<Value>& args) override;
};

struct BuiltinDictionaryMerge : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "dictionary_merge";}
    Value invoke(const std::vector<Value>& args) override;
};

struct BuiltinDictionaryDestroy : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "dictionary_destroy";}
    Value invoke(const std::vector<Value>& args) override;
};

void register_builtin_dictionary();


#endif //BERESTALANGUAGE_BUILTINDICTIONARY_H
