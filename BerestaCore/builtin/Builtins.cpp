//
// Created by Denis on 14.07.2025.
//

#include "Builtins.h"
#include "Matrix2D.h"
#include <iostream>
#include <cmath>
#include <random>

static Matrix2D current_matrix = Matrix2D::identity();

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
                if(args.size() != 3 ||
                   (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER) ||
                   (args[1].type != ValueType::DOUBLE && args[1].type != ValueType::INTEGER) ||
                   (args[2].type != ValueType::DOUBLE && args[2].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] clamp expects 3 numeric arguments (val, min, max)\n"; return {};
                }
                double val = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : std::get<int>(args[0].data);
                double min = (args[1].type == ValueType::DOUBLE) ? std::get<double>(args[1].data) : std::get<int>(args[1].data);
                double max = (args[2].type == ValueType::DOUBLE) ? std::get<double>(args[2].data) : std::get<int>(args[2].data);
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
            "choose_from", [](const std::vector<Value>& args) -> Value
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
        },

        // тригонометрия
        {
            "sin", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] sin expects 1 numeric argument (in radians)\n"; return {};
                }
                double radians = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                return Value(std::sin(radians));
            }
        },
        {
            "cos", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] cos expects 1 numeric argument (in radians)\n"; return {};
                }
                double radians = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                return Value(std::cos(radians));
            }
        },
        {
            "tan", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] tan expects 1 numeric argument (in radians)\n"; return {};
                }
                double radians = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                double result = std::tan(radians);
                if(std::abs(result) < 1e-5) {result = 0.0;}
                if(!std::isfinite(result)) {std::cerr << "[ERROR] tan argument results in infinity (asymptote)\n"; return {};}
                return Value(result);
            }
        },
        {
            "arcsin", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] arcsin expects 1 numeric argument\n";return {};
                }
                double radians = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                if(radians < -1.0 || radians > 1.0) {std::cerr << "[ERROR] arcsin domain error: input must be in range [-1, 1]\n"; return {};}
                return Value(std::asin(radians));
            }
        },
        {
            "arccos", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] arccos expects 1 numeric argument\n";return {};
                }
                double radians = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                if(radians < -1.0 || radians > 1.0) {std::cerr << "[ERROR] arccos domain error: input must be in range [-1, 1]\n"; return {};}
                return Value(std::acos(radians));
            }
        },
        {
            "arctan", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] arctan expects 1 numeric argument\n";return {};
                }
                double radians = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                return Value(std::atan(radians));
            }
        },
        {
            "arctan2", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 2 ||
                (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER) ||
                (args[1].type != ValueType::DOUBLE && args[1].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] arctan2 expects 2 numeric arguments (y, x)\n"; return {};
                }
                double y = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                double x = (args[1].type == ValueType::DOUBLE) ? std::get<double>(args[1].data) : static_cast<double>(std::get<int>(args[1].data));
                double result = std::atan2(y, x);
                result = std::round(result * 100.0) / 100.0;
                return Value(result);
            }
        },
        {
            "dsin", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] dsin expects 1 numeric argument (in degrees)\n"; return {};
                }
                double degrees = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                double radians = degrees * (M_PI / 180.0);
                double result = std::sin(radians);
                if(std::abs(result) < 1e-5) {result = 0.0;}
                result = std::round(result * 100.0) / 100.0;
                return Value(result);
            }
        },
        {
            "dcos", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] dcos expects 1 numeric argument (in degrees)\n"; return {};
                }
                double degrees = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                double radians = degrees * (M_PI / 180.0);
                double result = std::cos(radians);
                if(std::abs(result) < 1e-5) {result = 0.0;}
                result = std::round(result * 100.0) / 100.0;
                return Value(result);
            }
        },
        {
            "dtan", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] dtan expects 1 numeric argument (in degrees)\n"; return {};
                }
                double degrees = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                double radians = degrees * (M_PI / 180.0);
                double result = std::tan(radians);
                if(std::fabs(std::fmod(std::abs(degrees), 180.0) - 90.0) < 1e-6) {std::cerr << "[ERROR] dtan asymptote at " << degrees << " degrees\n"; return {};}
                if(!std::isfinite(result)) {std::cerr << "[ERROR] dtan argument results in infinity (asymptote)\n"; return {};}
                if(std::abs(result) < 1e-5) {result = 0.0;}
                result = std::round(result * 100.0) / 100.0;
                return Value(result);
            }
        },
        {
            "darcsin", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] darcsin expects 1 numeric argument in range [-1, 1]\n"; return {};
                }
                double val = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                if(val < -1.0 || val > 1.0) {std::cerr << "[ERROR] darcsin domain error: input must be in range [-1, 1]\n"; return {};}
                double radians = std::asin(val);
                double degrees = radians * (180.0 / M_PI);
                degrees = std::round(degrees * 100.0) / 100.0;
                return Value(degrees);
            }
        },
        {
            "darccos", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] darccos expects 1 numeric argument in range [-1, 1]\n"; return {};
                }
                double val = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                if(val < -1.0 || val > 1.0) {std::cerr << "[ERROR] darccos domain error: input must be in range [-1, 1]\n"; return {};}
                double radians = std::acos(val);
                double degrees = radians * (180.0 / M_PI);
                degrees = std::round(degrees * 100.0) / 100.0;
                return Value(degrees);
            }
        },
        {
            "darctan", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 1 || (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] darctan expects 1 numeric argument\n"; return {};
                }
                double val = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                double radians = std::atan(val);
                double degrees = radians * (180.0 / M_PI);
                degrees = std::round(degrees * 100.0) / 100.0;
                return Value(degrees);
            }
        },
        {
            "darctan2", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 2 ||
                   (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER) ||
                   (args[1].type != ValueType::DOUBLE && args[1].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] darctan2 expects 2 numeric arguments (y, x)\n"; return {};
                }
                double y = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                double x = (args[1].type == ValueType::DOUBLE) ? std::get<double>(args[1].data) : static_cast<double>(std::get<int>(args[1].data));
                double radians = std::atan2(y, x);
                double degrees = radians * (180.0 / M_PI);
                degrees = std::round(degrees * 100.0) / 100.0;
                return Value(degrees);
            }
        },
        {
            "point_direction", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 4) {std::cerr << "[ERROR] point_direction expects 4 arguments (x1, y1, x2, y2)\n"; return {};}
                for(int i = 0; i < 4; ++i)
                {
                    if(args[i].type != ValueType::DOUBLE && args[i].type != ValueType::INTEGER)
                    {
                        std::cerr << "[ERROR] point_direction argument " << i + 1 << " must be numeric\n"; return {};
                    }
                }
                auto to_double = [](const Value& v) -> double
                {
                    return (v.type == ValueType::DOUBLE) ? std::get<double>(v.data) : static_cast<double>(std::get<int>(v.data));
                };
                double x1 = to_double(args[0]);
                double y1 = to_double(args[1]);
                double x2 = to_double(args[2]);
                double y2 = to_double(args[3]);
                double dx = x2 - x1;
                double dy = y2 - y1;
                double radians = std::atan2(dy, dx);
                double degrees = radians * (180.0 / M_PI);
                degrees = std::round(degrees * 100.0) / 100.0;
                return Value(degrees);
            }
        },
        {
            "point_distance", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 4) {std::cerr << "[ERROR] point_distance expects 4 arguments (x1, y1, x2, y2)\n"; return {};}
                for(int i = 0; i < 4; ++i)
                {
                    if(args[i].type != ValueType::DOUBLE && args[i].type != ValueType::INTEGER)
                    {
                        std::cerr << "[ERROR] point_distance argument " << i + 1 << " must be numeric\n"; return {};
                    }
                }
                auto to_double = [](const Value& v) -> double
                {
                    return (v.type == ValueType::DOUBLE) ? std::get<double>(v.data) : static_cast<double>(std::get<int>(v.data));
                };
                double x1 = to_double(args[0]);
                double y1 = to_double(args[1]);
                double x2 = to_double(args[2]);
                double y2 = to_double(args[3]);
                double dx = x2 - x1;
                double dy = y2 - y1;
                double distance = std::sqrt(dx * dx + dy * dy);
                distance = std::round(distance * 100.0) / 100.0;
                return Value(distance);
            }
        },
        {
            "lengthdir_x", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 2 ||
                   (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER) ||
                   (args[1].type != ValueType::DOUBLE && args[1].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] lengthdir_x expects 2 numeric arguments\n"; return {};
                }
                double len = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                double dir = (args[1].type == ValueType::DOUBLE) ? std::get<double>(args[1].data) : static_cast<double>(std::get<int>(args[1].data));
                double radians = dir * (M_PI / 180.0);
                double x = len * std::cos(radians);
                return Value(x);
            }
        },
        {
            "lengthdir_y", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 2 ||
                   (args[0].type != ValueType::DOUBLE && args[0].type != ValueType::INTEGER) ||
                   (args[1].type != ValueType::DOUBLE && args[1].type != ValueType::INTEGER))
                {
                    std::cerr << "[ERROR] lengthdir_y expects 2 numeric arguments\n"; return {};
                }
                double len = (args[0].type == ValueType::DOUBLE) ? std::get<double>(args[0].data) : static_cast<double>(std::get<int>(args[0].data));
                double dir = (args[1].type == ValueType::DOUBLE) ? std::get<double>(args[1].data) : static_cast<double>(std::get<int>(args[1].data));
                double radians = dir * (M_PI / 180.0);
                double y = -len * std::sin(radians);
                return Value(y);
            }
        },

        // матрицы
        {
            "matrix_get", [](const std::vector<Value>& args) -> Value
            {
                std::vector<Value> result;
                for(int i = 0; i < 6; ++i)
                {
                    result.emplace_back(current_matrix.data[i]);
                }
                return Value(result);
            }
        },
        {
            "matrix_set", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 6) {std::cerr << "[ERROR] matrix_set expects 6 numeric arguments\n"; return {};}
                std::array<double, 6> new_data{};
                for(int i = 0; i < 6; ++i)
                {
                    if(args[i].type != ValueType::DOUBLE && args[i].type != ValueType::INTEGER)
                    {
                        std::cerr << "[ERROR] matrix_set argument " << i + 1 << " must be numeric\n"; return {};
                    }
                    new_data[i] = (args[i].type == ValueType::DOUBLE) ? std::get<double>(args[i].data) : static_cast<double>(std::get<int>(args[i].data));
                }
                current_matrix.data = new_data;
                return Value(true);
            }
        },
        {
            "matrix_build", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 5)
                {
                    std::cerr << "[ERROR] matrix_build expects 5 arguments (scale_x, scale_y, rotation_deg, translate_x, translate_y)\n"; return {};
                }
                auto get_number = [](const Value& val) -> double
                {
                    if(val.type == ValueType::DOUBLE) return std::get<double>(val.data);
                    if(val.type == ValueType::INTEGER) return static_cast<double>(std::get<int>(val.data));
                    throw std::runtime_error("Expected number (int or double)");
                };
                double sx = get_number(args[0]);
                double sy = get_number(args[1]);
                double rot = get_number(args[2]);
                double tx = get_number(args[3]);
                double ty = get_number(args[4]);
                current_matrix = Matrix2D::build(sx, sy, rot, tx, ty);
                std::vector<Value> result;
                for(int i = 0; i < 6; ++i) {result.emplace_back(current_matrix.data[i]);}
                return Value(result);
            }
        },
        {
            "matrix_identity", [](const std::vector<Value>& args) -> Value
            {
                current_matrix = Matrix2D::identity();
                std::vector<Value> result;
                for(int i = 0; i < 6; ++i) {result.emplace_back(current_matrix.data[i]);}
                return Value(result);
            }
        },
        {
            "matrix_multiply", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 6)
                {
                    std::cerr << "[ERROR] matrix_multiply expects 6 arguments (a, b, c, d, tx, ty)\n";
                    return {};
                }
                auto get_number = [](const Value& val) -> double
                {
                    if(val.type == ValueType::DOUBLE) return std::get<double>(val.data);
                    if(val.type == ValueType::INTEGER) return static_cast<double>(std::get<int>(val.data));
                    throw std::runtime_error("Expected number (int or double)");
                };
                Matrix2D other{{
                    get_number(args[0]),
                    get_number(args[1]),
                    get_number(args[2]),
                    get_number(args[3]),
                    get_number(args[4]),
                    get_number(args[5])
                }};
                current_matrix = current_matrix.multiply(other);
                std::vector<Value> result;
                for(int i = 0; i < 6; ++i)
                {
                    result.emplace_back(current_matrix.data[i]);
                }
                return Value(result);
            }
        },
        {
            "matrix_inverse", [](const std::vector<Value>& args) -> Value
            {
                if(!args.empty()) {std::cerr << "[ERROR] matrix_inverse expects no arguments\n"; return {};}
                try
                {
                    current_matrix = current_matrix.inverse();
                }
                catch(const std::exception& e)
                {
                    std::cerr << "[ERROR] matrix_inverse failed: " << e.what() << "\n"; return {};
                }
                std::vector<Value> result;
                for(int i = 0; i < 6; ++i)
                {
                    result.emplace_back(current_matrix.data[i]);
                }
                return Value(result);
            }
        },
        {
            "matrix_transform_vertex", [](const std::vector<Value>& args) -> Value
            {
                if(args.size() != 2) {std::cerr << "[ERROR] matrix_transform_vertex expects 2 arguments (x, y)\n";return {};}
                auto get_number = [](const Value& val) -> double
                {
                    if(val.type == ValueType::DOUBLE) return std::get<double>(val.data);
                    if(val.type == ValueType::INTEGER) return static_cast<double>(std::get<int>(val.data));
                    throw std::runtime_error("Expected number (int or double)");
                };
                double x = get_number(args[0]);
                double y = get_number(args[1]);
                auto [tx, ty] = current_matrix.transform_vertex(x, y);
                return Value(std::vector<Value>{Value(tx), Value(ty)});
            }
        }

};
