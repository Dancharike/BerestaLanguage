//
// Created by Denis on 08.07.2025.
//

#ifndef BERESTALANGUAGE_INTERPRETER_H
#define BERESTALANGUAGE_INTERPRETER_H

#pragma once
#include "../parser/Statement.h"
#include "../parser/Evaluator.h"
#include "../environment/Environment.h"
#include "../interpreter/FunctionIndex.h"
#include "../diagnostics/Diagnostics.h"
#include "../diagnostics/BaseContext.h"
#include <unordered_map>
#include <string>
#include <vector>

struct FileUnit
{
    std::vector<std::unique_ptr<Statement>> ast;
};

class Interpreter : public BaseContext
{
    public:
        Interpreter(Environment& env, FunctionIndex& index, Diagnostics& diag);

        void register_file(const std::string& filename, const std::string& code);
        void execute_file(const std::string& filename);
        void run_project(const std::string& entry_file);

    private:
        void index_functions(const std::string& filename, const std::vector<std::unique_ptr<Statement>>& statements);
        Value interpret(const std::vector<std::unique_ptr<Statement>>& statements, const std::string& current_file);

        Environment _env;
        FunctionIndex _index;
        std::unordered_map<std::string, FileUnit> _files;
};

#endif //BERESTALANGUAGE_INTERPRETER_H
