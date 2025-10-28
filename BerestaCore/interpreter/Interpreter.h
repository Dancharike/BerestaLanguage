//
// Created by Denis on 08.07.2025.
//

#ifndef BERESTALANGUAGE_INTERPRETER_H
#define BERESTALANGUAGE_INTERPRETER_H

#pragma once
#include "api/Export.h"
#include "../interpreter/FunctionIndex.h"
#include "../module/ModuleManager.h"
#include "../runtime/environment/Environment.h"
#include "../frontend/diagnostics/Diagnostics.h"
#include "../frontend/diagnostics/BaseContext.h"
#include <unordered_map>
#include <string>
#include <vector>

class BERESTA_API Interpreter : public BaseContext
{
    public:
        Interpreter(Environment& env, FunctionIndex& index, Diagnostics& diag);

        void register_file(const std::string& filename, const std::string& code);
        void run_project(const std::string& entry_file);

    private:
        Environment& _env;
        FunctionIndex& _index;
        ModuleManager _modules;
};

#endif //BERESTALANGUAGE_INTERPRETER_H
