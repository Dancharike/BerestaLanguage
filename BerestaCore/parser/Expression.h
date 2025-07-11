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
    UNARY,
    IF
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

struct IfExpr : public Expression
{
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Expression> then_branch;
    std::unique_ptr<Expression> else_branch;

    IfExpr(std::unique_ptr<Expression> cond, std::unique_ptr<Expression> then_b, std::unique_ptr<Expression> else_b = nullptr)
        : Expression(ExpressionType::IF), condition(std::move(cond)), then_branch(std::move(then_b)), else_branch(std::move(else_b)) {}
};

#endif //BERESTALANGUAGE_EXPRESSION_H
