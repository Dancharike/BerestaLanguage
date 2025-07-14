//
// Created by Denis on 14.07.2025.
//

#include "Builtins.h"
#include <iostream>
#include <cmath>

const std::unordered_map<std::string, BuiltinFunction> builtin_functions =
{
        {
            "console_print", [](const std::vector<Value>& args) -> Value
            {
                for(const auto& val : args)
                {
                    std::cout << val.to_string() << " ";
                }
                std::cout << std::endl;
                return {};
            }
            },
            {
            "sqrt", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] sqrt expects 1 numeric argument\n";
                    return {};
                }
                double num = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data): std::get<int>(args[0].data);
                return Value(std::sqrt(num));
            }
        }
};