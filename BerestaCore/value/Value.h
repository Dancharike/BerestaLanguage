//
// Created by Denis on 08.07.2025.
//

#ifndef BERESTALANGUAGE_VALUE_H
#define BERESTALANGUAGE_VALUE_H

#pragma once
#include <variant>
#include <string>
#include <iostream>

enum class ValueType
{
    INTEGER,
    DOUBLE,
    BOOLEAN,
    STRING,
    NONE
};

class Value
{
    public:
        ValueType type;
        std::variant<int, double, bool, std::string> data;

        Value();
        explicit Value(int val);
        explicit Value(double val);
        explicit Value(bool val);
        explicit Value(const std::string& val);

        [[nodiscard]] std::string to_string() const;
};


#endif //BERESTALANGUAGE_VALUE_H
