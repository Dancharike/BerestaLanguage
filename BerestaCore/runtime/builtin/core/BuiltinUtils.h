//
// Created by Denis on 09.10.2025.
//

#ifndef BERESTALANGUAGE_BUILTINUTILS_H
#define BERESTALANGUAGE_BUILTINUTILS_H

#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include "runtime/value/Value.h"

inline bool is_numeric(const Value& value) {return value.type == ValueType::INTEGER || value.type == ValueType::DOUBLE;}

inline void diag_error(Diagnostics& diag, const std::string& file, int line, const std::string& msg)
{
    diag.error(msg, file, line);
}

inline void ensure_arity(const std::vector<Value>& args, size_t min_arity, size_t max_arity = SIZE_MAX)
{
    if(args.size() < min_arity || args.size() > max_arity) {throw std::runtime_error("Invalid number of arguments");}
}

inline bool ensure_min_arity(Diagnostics& diag, const std::string& file, int line, const std::vector<Value>& args, size_t min)
{
    if(args.size() < min)
    {
        diag.error("Function expects at least " + std::to_string(min) + " argument(s)", file, line);
        return false;
    }
    return true;
}

inline bool check_arity(Diagnostics& diag, const std::string& file, int line, const std::vector<Value>& args, size_t expected, const char* func_name)
{
    if(args.size() != expected)
    {
        diag.error(std::string(func_name) + " expects " + std::to_string(expected) + " argument(s)", file, line);
        return false;
    }
    return true;
}

inline bool check_arity_range(Diagnostics& diag, const std::string& file, int line, const std::vector<Value>& args, size_t min_expected, const char* func_name)
{
    if(args.size() < min_expected)
    {
        diag.error(std::string(func_name) + " expects at least " + std::to_string(min_expected) + " argument(s)", file, line);
        return false;
    }
    return true;
}

inline bool check_numeric(Diagnostics& diag, const std::string& file, int line, const Value& v, const char* func_name, int index1based)
{
    if(!is_numeric(v))
    {
        diag.error(std::string(func_name) + ": argument #" + std::to_string(index1based) + " must be numeric", file, line);
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
