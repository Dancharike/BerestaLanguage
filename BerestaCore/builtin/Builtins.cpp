//
// Created by Denis on 14.07.2025.
//

#include "Builtins.h"
#include <iostream>
#include <cmath>
#include <random>

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
            "sqr", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] sqr expects 1 numeric argument\n"; return {};
                }
                double num = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : std::get<int>(args[0].data);
                return Value(num * num);
            }
        },
        {
            "sqrt", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] sqrt expects 1 numeric argument\n"; return {};
                }
                double num = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : std::get<int>(args[0].data);
                return Value(std::sqrt(num));
            }
        },
        {
            "abs", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1) {std::cerr << "[ERROR] abs expects 1 argument\n"; return {};}
                if(args[0].type == ValueType::INTEGER) {return Value(std::abs(std::get<int>(args[0].data)));}
                if(args[0].type == ValueType::DOUBLE) {return Value(std::abs(std::get<double>(args[0].data)));}
                std::cerr << "[ERROR] abs: unsupported type\n";
                return {};
            }
        },
        {
            "round", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] round expects 1 numeric argument\n"; return {};
                }
                double val = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                return Value(std::round(val));
            }
        },
        {
            "floor", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] floor expects 1 numeric argument\n"; return {};
                }
                double val = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                return Value(std::floor(val));
            }
        },
        {
            "ceil", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] ceil expects 1 numeric argument\n"; return {};
                }
                double val = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                return Value(std::ceil(val));
            }
        },
        {
            "frac", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 ||(args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER)) {std::cerr << "[ERROR] frac expects 1 numeric argument\n"; return {};}
                double val = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                return Value(val - std::floor(val));
            }
        },
        {
            "min", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 2) {std::cerr << "[ERROR] min expects 2 arguments\n"; return {};}
                double a = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : std::get<int>(args[0].data);
                double b = (args[1].type == ValueType::DOUBLE) ? std::get<double>(args[1].data) : std::get<int>(args[1].data);
                return Value(std::min(a, b));
            }
        },
        {
            "max", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 2) {std::cerr << "[ERROR] max expects 2 arguments\n"; return {};}
                double a = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : std::get<int>(args[0].data);
                double b = (args[1].type == ValueType::DOUBLE) ? std::get<double>(args[1].data) : std::get<int>(args[1].data);
                return Value(std::max(a, b));
            }
        },
        {
            "mean", [](const std::vector<Value>& args) -> Value
            {
                if(args.empty()) {std::cerr << "[ERROR] mean expects at least 1 argument\n"; return {};}
                double sum = 0;
                int count = 0;
                for(const auto& val : args)
                {
                    if(val.type == ValueType::INTEGER) {sum += std::get<int>(val.data);}
                    else if(val.type == ValueType::DOUBLE) {sum += std::get<double>(val.data);}
                    else {std::cerr << "[ERROR] mean: unsupported type\n"; return {};}
                    count++;
                }
                return Value(sum / count);
            }
        },
        {
            "median", [](const std::vector<Value>& args) -> Value
            {
                if(args.empty()) {std::cerr << "[ERROR] median expects at least 1 argument\n"; return {};}
                std::vector<double> values;
                for(const auto& val : args)
                {
                    if(val.type == ValueType::INTEGER) {values.push_back(std::get<int>(val.data));}
                    else if(val.type == ValueType::DOUBLE) {values.push_back(std::get<double>(val.data));}
                    else {std::cerr << "[ERROR] median: unsupported type\n"; return {};}
                }
                std::sort(values.begin(), values.end());
                size_t mid = values.size() / 2;
                return Value(values[mid]);
            }
        },
        {
            "power", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 2) {std::cerr << "[ERROR] pow expects 2 arguments\n"; return {};}
                double base = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : std::get<int>(args[0].data);
                double exp = (args[1].type == ValueType::DOUBLE) ? std::get<double>(args[1].data) : std::get<int>(args[1].data);
                return Value(std::pow(base, exp));
            }
        },
        {
            "ln", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] ln expects 1 numeric argument\n"; return {};
                }
                double val = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                if(val <= 0.0) {std::cerr << "[ERROR] ln: input must be > 0\n"; return {};}
                return Value(std::log(val));
            }
        },
        {
            "log2", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] log2 expects 1 numeric argument\n"; return {};
                }
                double val = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                if(val <= 0.0) {std::cerr << "[ERROR] log2 domain error: input must be positive\n"; return {};}
                return Value(std::log2(val));
            }
        },
        {
            "log10", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] log10 expects 1 numeric argument\n"; return {};
                }
                double val = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                if(val <= 0.0) {std::cerr << "[ERROR] log10 domain error: input must be positive\n"; return {};}
                return Value(std::log10(val));
            }
        },
        {
            "logn", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 2 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER) ||
                (args[1].type != ValueType::DOUBLE && args[1].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] logn expects 2 numeric arguments: logn(base, value)\n"; return {};
                }
                double base = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                double val = (args[1].type == ValueType::DOUBLE) ? std::get<double>(args[1].data) : static_cast<double>(std::get<int>(args[1].data));
                if(base <= 0.0 || base == 1.0 || val <= 0.0) {std::cerr << "[ERROR] logn domain error: base must be > 0 and ≠ 1, value must be > 0\n"; return {};}
                return Value(std::log(val) / std::log(base));
            }
        },
        {
            "clamp", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 3 ||(args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER) ||
                    args[1].type != ValueType::DOUBLE || args[2].type != ValueType::DOUBLE)
                {
                    std::cerr << "[ERROR] clamp expects 3 numeric arguments (val, min, max)\n"; return {};
                }
                double val = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : std::get<int>(args[0].data);
                double min = std::get<double>(args[1].data);
                double max = std::get<double>(args[2].data);
                return Value(std::clamp(val, min, max));
            }
        },
        {
            "lerp", [](const std::vector<Value>& args) -> Value
            {
                if (args.size() != 3 || args[0].type != ValueType::DOUBLE || args[1].type != ValueType::DOUBLE || args[2].type != ValueType::DOUBLE)
                {
                    std::cerr << "[ERROR] lerp expects 3 double arguments (a, b, amount)\n"; return {};
                }
                double a = std::get<double>(args[0].data);
                double b = std::get<double>(args[1].data);
                double amt = std::get<double>(args[2].data);
                return Value(a + (b - a) * amt);
            }
        },
        {
            "choose", [](const std::vector<Value>& args) -> Value
            {
                if(args.empty()) {std::cerr << "[ERROR] choose expects at least 1 argument\n"; return {};}
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_int_distribution<size_t> dist(0, args.size() - 1);
                return args[dist(gen)];
            }
        },
        {
            "random", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER)) {std::cerr << "[ERROR] random expects 1 numeric argument\n"; return {};}
                static std::random_device rd;
                static std::mt19937 gen(rd());
                double upper = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                if(upper <= 0.0) {std::cerr << "[ERROR] random upper bound must be > 0\n"; return {};}
                std::uniform_real_distribution<double> dist(0.0, upper);
                return Value(dist(gen));
            }
        },
        {
            "random_range", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 2 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER) ||
                (args[1].type != ValueType::DOUBLE && args[1].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] random_range expects 2 numeric arguments (min, max)\n"; return {};
                }
                double min = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : std::get<int>(args[0].data);
                double max = (args[1].type == ValueType::DOUBLE) ? std::get<double>(args[1].data) : std::get<int>(args[1].data);
                if(min > max) std::swap(min, max);
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_real_distribution<double> dist(min, max);
                return Value(dist(gen));
            }
        },
        {
            "int_random", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER)) {std::cerr << "[ERROR] int_random expects 1 numeric argument\n"; return {};}
                double upper_raw = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : std::get<int>(args[0].data);
                int upper = static_cast<int>(std::floor(upper_raw));
                if(upper < 0) {std::cerr << "[ERROR] int_random: upper bound must be >= 0\n"; return {};}
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_int_distribution<int> dist(0, upper);
                return Value(dist(gen));
            }
        },
        {
            "int_random_range", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 2 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER) || (args[1].type != ValueType::DOUBLE && args[1].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] int_random_range expects 2 numeric arguments\n";return {};
                }
                int min_val = static_cast<int>((args[0].type == ValueType::DOUBLE) ? std::floor(std::get<double>(args[0].data)) : std::get<int>(args[0].data));
                int max_val = static_cast<int>((args[1].type == ValueType::DOUBLE) ? std::floor(std::get<double>(args[1].data)) : std::get<int>(args[1].data));
                if(min_val > max_val) {std::swap(min_val, max_val);}
                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_int_distribution<int> dist(min_val, max_val);
                return Value(dist(gen));
            }
        }

};
