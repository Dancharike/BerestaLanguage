//
// Created by Denis on 12.10.2025.
//

#ifndef BERESTALANGUAGE_MODULE_H
#define BERESTALANGUAGE_MODULE_H

#pragma once
#include <memory>
#include <string>
#include "../parser/Evaluator.h"
#include "../parser/Statement.h"
#include "../diagnostics/Diagnostics.h"

class Module : BaseContext
{
    public:
        Module(std::string filename, Environment* env, FunctionIndex& index, Diagnostics& diag);

        void set_ast(std::vector<std::unique_ptr<Statement>> stmts);
        [[nodiscard]] const std::vector<std::unique_ptr<Statement>>& get_ast() const;
        [[nodiscard]] Environment& environment();
        [[nodiscard]] FunctionIndex& index();

    private:
        std::string _filename;
        Environment* _env;
        FunctionIndex& _index;
        std::vector<std::unique_ptr<Statement>> _ast;
};


#endif //BERESTALANGUAGE_MODULE_H
