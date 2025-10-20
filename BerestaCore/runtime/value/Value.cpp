//
// Created by Denis on 08.07.2025.
//

#include "Value.h"
#include "runtime/value/StructValue.h"
#include <memory>
#include <sstream>
#include <iomanip>

Value::Value() : type(ValueType::NONE), data(std::monostate{}) {}

Value::Value(int val) : type(ValueType::INTEGER), data(val) {}
Value::Value(double val) : type(ValueType::DOUBLE), data(val) {}
Value::Value(bool val) : type(ValueType::BOOLEAN), data(val) {}
Value::Value(const std::string& val) : type(ValueType::STRING), data(val) {}
Value::Value(const std::vector<Value>& val) : type(ValueType::ARRAY), data(val) {}
Value::Value(const Dictionary& val) : type(ValueType::DICTIONARY), data(std::make_shared<Dictionary>(val)) {}
Value::Value(const StructInstance& val) : type(ValueType::STRUCT), data(std::make_shared<StructInstance>(val)) {}

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

        case ValueType::BOOLEAN: {return std::get<bool>(data) ? "true" : "false";}

        case ValueType::STRING: {return std::get<std::string>(data);}

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

        case ValueType::STRUCT:
        {
            const auto& ptr = std::get<std::shared_ptr<StructInstance>>(data);
            const auto& inst = *ptr;
            if(!inst.definition) {return "{ }";}

            std::string s = "{ ";
            for(size_t i = 0; i < inst.definition->field_names.size(); ++i)
            {
                const auto& name = inst.definition->field_names[i];
                auto it = inst.fields.find(name);
                s += name + ": " + (it != inst.fields.end() ? it->second.to_string() : "none");
                if(i + 1 < inst.definition->field_names.size()) {s += ", ";}
            }
            s += " }";
            return s;
        }

        /*
        case ValueType::DICTIONARY:
        {
            const auto& dict = *std::get<DictionaryPtr>(data);
            std::string result = "{";
            size_t count = 0;
            for(const auto& [key, val] : dict)
            {
                result += "\"" + key + "\": " + val.to_string();
                if(++count < dict.size()) {result += ", ";}
            }
            result += "}";
            return result;
        }
        */
        case ValueType::NONE: return "none";

        default: return "none (no return)";
    }
}