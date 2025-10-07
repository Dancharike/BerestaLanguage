//
// Created by Denis on 09.07.2025.
//

#ifndef BERESTALANGUAGE_EVALUATOR_H
#define BERESTALANGUAGE_EVALUATOR_H

#pragma once
#include "Expression.h"
#include "Statement.h"
#include "../value/Value.h"
#include "../environment/Environment.h"
#include "../diagnostics/Diagnostics.h"
#include "../diagnostics/BaseContext.h"
#include <unordered_map>
#include <string>
#include <stack>

struct ReturnException
{
    Value value;

    explicit ReturnException(Value v) : value(std::move(v)) {}
};

class FunctionIndex;

class Evaluator : public BaseContext
{
    public:
        Evaluator(Environment& env, FunctionIndex& index, std::string current_file, Diagnostics& _diag);

        Value eval_expression(Expression* expr);
        Value eval_statement(Statement* stmt);

    private:
        Environment& _env;
        FunctionIndex& _index;
        std::vector<std::string> _file_stack;
};


#endif
