//
// Created by Denis on 08.07.2025.
//

#ifndef BERESTALANGUAGE_STATEMENT_H
#define BERESTALANGUAGE_STATEMENT_H

#pragma once
#include "../value/Value.h"
#include "Expression.h"
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
    std::unique_ptr<Expression> value;

    Assignment(std::string name, std::unique_ptr<Expression> value): name(std::move(name)), value(std::move(value))
    {
        type = StatementType::ASSIGNMENT;
    }
};

#endif //BERESTALANGUAGE_STATEMENT_H
