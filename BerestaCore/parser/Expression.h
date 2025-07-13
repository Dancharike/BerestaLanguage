//
// Created by Denis on 09.07.2025.
//

#ifndef BERESTALANGUAGE_EXPRESSION_H
#define BERESTALANGUAGE_EXPRESSION_H

#pragma once
#include "../value/Value.h"
#include "Statement.h"
#include <memory>
#include <string>
#include <vector>

struct Statement;

enum class ExpressionType
{
    NUMBER,
    BINARY,
    VARIABLE,
    UNARY,
    CONSOLE_PRINT
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
    explicit NumberExpr(double number) : Expression(ExpressionType::NUMBER), value(number) {}
};

struct BinaryExpr : public Expression
{
    //char op;  // пришлось заменить на строчку, ибо в противном случае невозможно будет реализовать операторы сравнения
    std::string op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    BinaryExpr(std::string op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : Expression(ExpressionType::BINARY), op(std::move(op)), left(std::move(left)), right(std::move(right)) {}
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

struct ConsolePrintExpr : public Expression
{
    std::unique_ptr<Expression> expression;

    explicit ConsolePrintExpr(std::unique_ptr<Expression> expr) : Expression(ExpressionType::CONSOLE_PRINT), expression(std::move(expr)) {}
};

#endif //BERESTALANGUAGE_EXPRESSION_H
