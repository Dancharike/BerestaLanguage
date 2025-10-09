//
// Created by Denis on 09.10.2025.
//

#ifndef BERESTALANGUAGE_BUILTINREGISTRY_H
#define BERESTALANGUAGE_BUILTINREGISTRY_H

#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include "IBuiltinFunction.h"

class BuiltinRegistry
{
    public:
        static BuiltinRegistry& instance();
        void register_builtin(std::unique_ptr<IBuiltinFunction> func);
        IBuiltinFunction* get(const std::string& name);
        void clear();

    private:
        BuiltinRegistry() = default;
        std::unordered_map<std::string, std::unique_ptr<IBuiltinFunction>> _builtins;
};

void register_default_builtins();


#endif //BERESTALANGUAGE_BUILTINREGISTRY_H
