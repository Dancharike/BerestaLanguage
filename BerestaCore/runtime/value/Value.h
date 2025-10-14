//
// Created by Denis on 08.07.2025.
//

#ifndef BERESTALANGUAGE_VALUE_H
#define BERESTALANGUAGE_VALUE_H

#pragma once
#include <variant>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>

enum class ValueType
{
    INTEGER,
    DOUBLE,
    BOOLEAN,
    STRING,
    ARRAY,
    DICTIONARY,
    NONE
};

struct Value;
using Dictionary = std::unordered_map<std::string, Value>;
using DictionaryPtr = std::shared_ptr<Dictionary>;

class Value
{
    public:
        ValueType type;
        std::variant<std::monostate, int, double, bool, std::string, std::vector<Value>, DictionaryPtr> data;

        Value();
        explicit Value(int val);
        explicit Value(double val);
        explicit Value(bool val);
        explicit Value(const std::string& val);
        explicit Value(const std::vector<Value>& val);
        explicit Value(const Dictionary& val);

        [[nodiscard]] std::string to_string() const;
};


#endif //BERESTALANGUAGE_VALUE_H
