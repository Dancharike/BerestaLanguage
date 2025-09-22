//
// Created by Denis on 08.07.2025.
//

#ifndef BERESTALANGUAGE_INTERPRETER_H
#define BERESTALANGUAGE_INTERPRETER_H

#pragma once
#include "../parser/Statement.h"
#include "../parser/Evaluator.h"
#include <unordered_map>
#include <string>
#include <vector>

struct FileUnit
{
    std::vector<std::unique_ptr<Statement>> ast;
    std::unordered_map<std::string, FunctionStatement*> private_functions;
};

class Interpreter
{
    public:
        void register_file(const std::string& filename, const std::string& code);
        void execute_file(const std::string& filename);
        void run_project(const std::string& entry_file);

    private:
        std::pair<std::string, Value> interpret(const std::vector<std::unique_ptr<Statement>>& statements);
        void index_functions(const std::string& filename, const std::vector<std::unique_ptr<Statement>>& statements);

        std::unordered_map<std::string, Value> variables;

        std::unordered_map<std::string, FileUnit> files;

        PublicFunctionMap public_functions;
        PrivateFunctionMap private_functions_by_file;
};

#endif //BERESTALANGUAGE_INTERPRETER_H
