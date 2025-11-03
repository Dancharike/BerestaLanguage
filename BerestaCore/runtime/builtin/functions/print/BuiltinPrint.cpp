//
// Created by Denis on 09.10.2025.
//

#include "BuiltinPrint.h"
#include "runtime/builtin/core/BuiltinRegistry.h"
#include "runtime/environment/Environment.h"
#include <iostream>

extern Environment* get_active_environment();

Value BuiltinPrint::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    Environment* env = get_active_environment();
    std::ostream& out = env ? env->out() : std::cout;

    if(!args.empty())
    {
        for(size_t i = 0; i < args.size(); ++i)
        {
            out << args[i].to_string();
            if(i + 1 < args.size()) {out << " ";}
        }

        out << "\n";
        return args.back();
    }

    out << "\n";
    return {};
}

void register_builtin_console_print()
{
    BuiltinRegistry::instance().register_builtin(std::make_unique<BuiltinPrint>());
}
