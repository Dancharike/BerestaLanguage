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

enum class ValueType
{
    INTEGER,
    DOUBLE,
    BOOLEAN,
    STRING,
    ARRAY,
    NONE
};

class Value
{
    public:
        ValueType type;
        std::variant<int, double, bool, std::string, std::vector<Value>> data;

        Value();
        explicit Value(int val);
        explicit Value(double val);
        explicit Value(bool val);
        explicit Value(const std::string& val);
        explicit Value(const std::vector<Value>& val);

        [[nodiscard]] std::string to_string() const;
};


#endif //BERESTALANGUAGE_VALUE_H
