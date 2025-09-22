//
// Created by Denis on 09.07.2025.
//

#ifndef BERESTALANGUAGE_EVALUATOR_H
#define BERESTALANGUAGE_EVALUATOR_H

#pragma once
#include "Expression.h"
#include "../value/Value.h"
#include <unordered_map>
#include <string>

struct ReturnException
{
    Value value;

    explicit ReturnException(Value v) : value(std::move(v)) {}
};

using PublicFunctionMap = std::unordered_map<std::string, std::pair<FunctionStatement*, std::string>>;
using PrivateFunctionMap = std::unordered_map<std::string, std::unordered_map<std::string, FunctionStatement*>>;

void set_public_functions(const PublicFunctionMap* ptr);
void set_private_functions(const PrivateFunctionMap* ptr);
void set_current_file(const std::string& filename);

Value evaluate(Expression* expr, const std::unordered_map<std::string, Value>& variables);
Value evaluate(Statement* stmt, std::unordered_map<std::string, Value>& variables);

#endif
