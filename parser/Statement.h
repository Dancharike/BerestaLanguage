//
// Created by Denis on 08.07.2025.
//

#ifndef BERESTALANGUAGE_STATEMENT_H
#define BERESTALANGUAGE_STATEMENT_H

#pragma once
#include "../value/Value.h"
#include <string>
#include <memory>

enum class StatementType
{
    ASSIGNMENT
};

struct Statement
{
    StatementType type;
    virtual ~Statement() = default;
};

struct Assignment : public Statement
{
    std::string name;
    Value value;

    Assignment(const std::string name, const Value& value) : name(name), value(value)
    {
        type = StatementType::ASSIGNMENT;
    }
};

#endif //BERESTALANGUAGE_STATEMENT_H
