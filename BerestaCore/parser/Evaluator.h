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
#include <stack>

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

static std::vector<std::unordered_map<std::string, Value>> env_stack = {{}};

static void push_scope() {env_stack.emplace_back();}
static void pop_scope() {env_stack.pop_back();}

static Value get_var(const std::string& name)
{
    for(int i = (int)env_stack.size() - 1; i >= 0; --i)
    {
        auto it = env_stack[i].find(name);
        if(it != env_stack[i].end()) {return it->second;}
    }

    std::cerr << "[ERROR] Variable not found: " << name << "\n";
    return {};
}

static void set_var(const std::string& name, const Value& v, bool is_let)
{
    if(is_let) {env_stack.back()[name] = v;}
    else
    {
        for(int i = (int)env_stack.size() - 1; i >= 0; --i)
        {
            auto it = env_stack[i].find(name);
            if(it != env_stack[i].end()) {env_stack[i][name] = v; return;}
        }

        env_stack[0][name] = v;
    }
}

Value evaluate(Expression* expr, const std::unordered_map<std::string, Value>& variables);
Value evaluate(Statement* stmt, std::unordered_map<std::string, Value>& variables);

#endif
