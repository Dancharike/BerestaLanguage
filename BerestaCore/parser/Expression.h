//
// Created by Denis on 09.07.2025.
//

#ifndef BERESTALANGUAGE_EXPRESSION_H
#define BERESTALANGUAGE_EXPRESSION_H

#pragma once
#include "../value/Value.h"
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
    DICTIONARY_LITERAL,
    INDEX,
    MEMBER_ACCESS
};

struct Expression
{
    ExpressionType type;
    int line;
    int column;

    explicit Expression(ExpressionType type, int line = -1, int column = -1);
    virtual ~Expression() = default;
};

struct NumberExpr : public Expression
{
    Value value;

    explicit NumberExpr(int number, int line = -1, int column = -1);
    explicit NumberExpr(double number, int line = -1, int column = -1);
};

struct BinaryExpr : public Expression
{
    std::string op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    BinaryExpr(std::string op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, int line = -1, int column = -1);
};

struct VariableExpr : public Expression
{
    std::string name;

    explicit VariableExpr(std::string name, int line = -1, int column = -1);
};

struct UnaryExpr : public Expression
{
    char op;
    std::unique_ptr<Expression> right;

    UnaryExpr(char op, std::unique_ptr<Expression> right, int line = -1, int column = -1);
};

struct StringExpr : public Expression
{
    std::string value;

    explicit StringExpr(std::string val, int line = -1, int column = -1);
};

struct BoolExpr : public Expression
{
    bool value;

    explicit BoolExpr(bool v, int line = -1, int column = -1);
};

struct FunctionCallExpr : public Expression
{
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> arguments;

    FunctionCallExpr(std::unique_ptr<Expression> expr, std::vector<std::unique_ptr<Expression>> args, int line = -1, int column = -1);
};

struct ArrayLiteralExpr : public Expression
{
    std::vector<std::unique_ptr<Expression>> elements;

    explicit ArrayLiteralExpr(std::vector<std::unique_ptr<Expression>> elems, int line = -1, int column = -1);
};

struct IndexExpr : public Expression
{
    std::unique_ptr<Expression> array;
    std::unique_ptr<Expression> index;

    IndexExpr(std::unique_ptr<Expression> arr, std::unique_ptr<Expression> idx, int line = -1, int column = -1);
};

struct MemberAccessExpr : public Expression
{
    std::unique_ptr<Expression> object;
    std::string member;

    MemberAccessExpr(std::unique_ptr<Expression> obj, std::string mem, int line = -1, int column = -1);
};

struct DictionaryLiteralExpr : public Expression
{
    std::vector<std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>> entries;

    explicit DictionaryLiteralExpr(std::vector<std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>> entr, int line = -1, int column = -1);
};

#endif //BERESTALANGUAGE_EXPRESSION_H
