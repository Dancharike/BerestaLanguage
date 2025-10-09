//
// Created by Denis on 09.10.2025.
//

#ifndef BERESTALANGUAGE_BUILTINUTILS_H
#define BERESTALANGUAGE_BUILTINUTILS_H

#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include "value/Value.h"

inline bool is_numeric(const Value& value) {return value.type == ValueType::INTEGER || value.type == ValueType::DOUBLE;}

inline void diag_error(const std::string& msg)
{
    std::cerr << "[ERROR] " << msg << "\n";
}

inline void ensure_arity(const std::vector<Value>& args, size_t min_arity, size_t max_arity = SIZE_MAX)
{
    if(args.size() < min_arity || args.size() > max_arity) {throw std::runtime_error("Invalid number of arguments");}
}

inline bool ensure_min_arity(const std::vector<Value>& args, size_t min)
{
    if(args.size() < min)
    {
        diag_error("Function expects at least " + std::to_string(min) + " argument(s)");
        return false;
    }
    return true;
}

inline bool check_arity(const std::vector<Value>& args, size_t expected, const char* func_name)
{
    if(args.size() != expected)
    {
        diag_error(std::string(func_name) + " expects " + std::to_string(expected) + " argument(s)");
        return false;
    }
    return true;
}

inline bool check_arity_range(const std::vector<Value>& args, size_t min_expected, const char* func_name)
{
    if(args.size() < min_expected)
    {
        diag_error(std::string(func_name) + " expects at least " + std::to_string(min_expected) + " argument(s)");
        return false;
    }
    return true;
}

inline bool check_numeric(const Value& v, const char* func_name, int index1based)
{
    if(!is_numeric(v))
    {
        diag_error(std::string(func_name) + ": argument #" + std::to_string(index1based) + " must be numeric");
        return false;
    }
    return true;
}

inline double as_double_lossy(const Value& value)
{
    switch(value.type)
    {
        case ValueType::DOUBLE:  {return std::get<double>(value.data);}
        case ValueType::INTEGER: {return static_cast<double>(std::get<int>(value.data));}
        default:                 {throw std::runtime_error("Expected numeric argument");}
    }
}

inline double num(const Value& v) {return as_double_lossy(v);}

inline int as_int_lossy(const Value& value)
{
    switch(value.type)
    {
        case ValueType::INTEGER: {return std::get<int>(value.data);}
        case ValueType::DOUBLE:  {return static_cast<int>(std::get<double>(value.data));}
        default:                 {throw std::runtime_error("Expected integer argument");}
    }
}


#endif //BERESTALANGUAGE_BUILTINUTILS_H
