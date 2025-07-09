//
// Created by Denis on 08.07.2025.
//

#include "Value.h"

Value::Value() : type(ValueType::NONE), data(0) {}

Value::Value(int val) : type(ValueType::INTEGER), data(val) {}
Value::Value(double val) : type(ValueType::DOUBLE), data(val) {}
Value::Value(bool val) : type(ValueType::BOOLEAN), data(val) {}
Value::Value(const std::string& val) : type(ValueType::STRING), data(val) {}

std::string Value::to_string() const
{
    switch(type)
    {
        case ValueType::INTEGER: return std::to_string(std::get<int>(data));
        case ValueType::DOUBLE: return std::to_string(std::get<double>(data));
        case ValueType::BOOLEAN : return std::get<bool>(data) ? "true" : "false";
        case ValueType::STRING : return  std::get<std::string>(data);
        default: return "none";
    }
}