//
// Created by Denis on 08.07.2025.
//

#include "Value.h"
#include <sstream>
#include <iomanip>

Value::Value() : type(ValueType::NONE), data(0) {}

Value::Value(int val) : type(ValueType::INTEGER), data(val) {}
Value::Value(double val) : type(ValueType::DOUBLE), data(val) {}
Value::Value(bool val) : type(ValueType::BOOLEAN), data(val) {}
Value::Value(const std::string& val) : type(ValueType::STRING), data(val) {}
Value::Value(const std::vector<Value>& val) : type(ValueType::ARRAY), data(val) {}

std::string Value::to_string() const
{
    switch(type)
    {
        case ValueType::INTEGER: return std::to_string(std::get<int>(data));

        case ValueType::DOUBLE:
        {
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(10);
            ss << std::get<double>(data);
            std::string result = ss.str();

            result.erase(result.find_last_not_of('0') + 1);
            if(result.back() == '.') result.pop_back();

            return result;
        }

        case ValueType::BOOLEAN: return std::get<bool>(data) ? "true" : "false";

        case ValueType::STRING: return std::get<std::string>(data);

        case ValueType::ARRAY:
        {
            const auto& arr = std::get<std::vector<Value>>(data);
            std::string result = "[";
            for(size_t i = 0; i < arr.size(); ++i)
            {
                result += arr[i].to_string();
                if(i + 1 < arr.size()) {result += ", ";}
            }
            result += "]";
            return result;
        }

        default: return "none (no return)";
    }
}