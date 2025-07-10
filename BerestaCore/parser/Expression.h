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
    BINARY,
    VARIABLE,
    UNARY
};

struct Expression
{
    ExpressionType type;
    explicit Expression(ExpressionType type) : type(type) {}
    virtual ~Expression() = default;
};

struct NumberExpr : public Expression
{
    Value value;

    explicit NumberExpr(int number) : Expression(ExpressionType::NUMBER), value(number) {}
};

struct BinaryExpr : public Expression
{
    char op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    BinaryExpr(char op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : Expression(ExpressionType::BINARY), op(op), left(std::move(left)), right(std::move(right)) {}
};

struct VariableExpr : public Expression
{
    std::string name;

    explicit VariableExpr(std::string name) : Expression(ExpressionType::VARIABLE), name(std::move(name)) {}
};

struct UnaryExpr : public Expression
{
    char op;
    std::unique_ptr<Expression> right;

    UnaryExpr(char op, std::unique_ptr<Expression> right): Expression(ExpressionType::UNARY), op(op), right(std::move(right)) {}
};

#endif //BERESTALANGUAGE_EXPRESSION_H
