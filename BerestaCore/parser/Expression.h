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
    STRING,
    BOOLEAN,
    FUNCTION_CALL,
    ARRAY_LITERAL,
    INDEX
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

struct StringExpr : public Expression
{
    std::string value;

    explicit StringExpr(std::string val) : Expression(ExpressionType::STRING), value(std::move(val)) {}
};

struct BoolExpr : public Expression
{
    bool value;

    explicit BoolExpr(bool v) : Expression(ExpressionType::BOOLEAN), value(v) {}
};

struct FunctionCallExpr : public Expression
{
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> arguments;

    FunctionCallExpr(std::unique_ptr<Expression> expr, std::vector<std::unique_ptr<Expression>> args)
        : Expression(ExpressionType::FUNCTION_CALL), callee(std::move(expr)), arguments(std::move(args)) {}
};

struct ArrayLiteralExpr : public Expression
{
    std::vector<std::unique_ptr<Expression>> elements;

    explicit ArrayLiteralExpr(std::vector<std::unique_ptr<Expression>> elems) : Expression(ExpressionType::ARRAY_LITERAL), elements(std::move(elems)) {}
};

struct IndexExpr : public Expression
{
    std::unique_ptr<Expression> array;
    std::unique_ptr<Expression> index;

    IndexExpr(std::unique_ptr<Expression> arr, std::unique_ptr<Expression> idx) : Expression(ExpressionType::INDEX), array(std::move(arr)), index(std::move(idx)) {}
};

#endif //BERESTALANGUAGE_EXPRESSION_H
