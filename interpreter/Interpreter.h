//
// Created by Denis on 08.07.2025.
//

#ifndef BERESTALANGUAGE_INTERPRETER_H
#define BERESTALANGUAGE_INTERPRETER_H

#pragma once
#include "../parser/Statement.h"
#include <unordered_map>
#include <string>
#include <vector>

class Interpreter
{
    public:
        std::pair<std::string, Value> interpret(const std::vector<std::unique_ptr<Statement>>& statements);
        void print_variables() const;

    private:
        std::unordered_map<std::string, Value> variables;
};

#endif //BERESTALANGUAGE_INTERPRETER_H
