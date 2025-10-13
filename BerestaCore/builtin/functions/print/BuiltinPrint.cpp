//
// Created by Denis on 09.10.2025.
//

#include "BuiltinPrint.h"
#include "builtin/core/BuiltinRegistry.h"
#include <iostream>

Value BuiltinPrint::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!args.empty())
    {
        for(size_t i = 0; i < args.size(); ++i)
        {
            std::cout << args[i].to_string();
            if(i + 1 < args.size()) {std::cout << " ";}
        }

        std::cout << "\n";
        return args.back();
    }

    std::cout << "\n";
    return {};
}

void register_builtin_console_print()
{
    BuiltinRegistry::instance().register_builtin(std::make_unique<BuiltinPrint>());
}
