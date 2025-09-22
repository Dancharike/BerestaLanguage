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

void set_user_functions(const std::unordered_map<std::string, FunctionStatement*>* ptr);

Value evaluate(Expression* expr, const std::unordered_map<std::string, Value>& variables);
Value evaluate(Statement* stmt, std::unordered_map<std::string, Value>& variables);

#endif
