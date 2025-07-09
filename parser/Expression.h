//
// Created by Denis on 09.07.2025.
//

#ifndef BERESTALANGUAGE_EXPRESSION_H
#define BERESTALANGUAGE_EXPRESSION_H

#pragma once
#include "../value/Value.h"
#include <memory>
#include <string>

enum class ExpressionType
{
    NUMBER,
    BINARY
};

struct Expression
{
    ExpressionType type;
    virtual ~Expression() = default;
};

struct NumberExpr : public Expression
{
    Value value;

    explicit NumberExpr(int number) : value(number)
    {
        type = ExpressionType::NUMBER;
    }
};

struct BinaryExpr : public Expression
{
    char op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    BinaryExpr(char op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right) : op(op), left(std::move(left)), right(std::move(right))
    {
        type = ExpressionType::BINARY;
    }
};

#endif //BERESTALANGUAGE_EXPRESSION_H
