//
// Created by Denis on 20.10.2025.
//

#ifndef BERESTALANGUAGE_STRUCTVALUE_H
#define BERESTALANGUAGE_STRUCTVALUE_H

#pragma once
#include "Value.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

struct StructFunction
{
    std::string name;
    bool is_public = true;
    std::function<Value(const std::vector<Value>&, std::unordered_map<std::string, Value>&)> invoker;
};

struct StructDefinition
{
    std::string name;
    std::vector<std::string> field_names;
    std::unordered_map<std::string, StructFunction> methods;
};

struct StructInstance
{
    std::shared_ptr<StructDefinition> definition;
    std::unordered_map<std::string, Value> fields;
};


#endif //BERESTALANGUAGE_STRUCTVALUE_H
