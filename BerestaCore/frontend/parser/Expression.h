//
// Created by Denis on 09.07.2025.
//

#ifndef BERESTALANGUAGE_EXPRESSION_H
#define BERESTALANGUAGE_EXPRESSION_H

#pragma once
#include "runtime/value/Value.h"
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

struct ExprVisitor;

struct Expression
{
    ExpressionType type;
    int line;
    int column;

    explicit Expression(ExpressionType type, int line = -1, int column = -1);
    virtual ~Expression() = default;

    virtual Value accept(ExprVisitor& val) = 0;

    [[nodiscard]] virtual std::string get_operator_value() const {return "";}
    [[nodiscard]] virtual char get_operator_char() const {return '?';}
    [[nodiscard]] virtual double get_number_value() const {return 0.0;}
};

struct NumberExpr : public Expression
{
    Value value;

    explicit NumberExpr(int number, int line = -1, int column = -1);
    explicit NumberExpr(double number, int line = -1, int column = -1);
    Value accept(ExprVisitor& val) override;

    [[nodiscard]] double get_number_value() const override;
};

struct BinaryExpr : public Expression
{
    std::string op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    BinaryExpr(std::string op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, int line = -1, int column = -1);
    Value accept(ExprVisitor& val) override;
};

struct VariableExpr : public Expression
{
    std::string name;

    explicit VariableExpr(std::string name, int line = -1, int column = -1);
    Value accept(ExprVisitor& val) override;
};

struct UnaryExpr : public Expression
{
    char op;
    std::unique_ptr<Expression> right;

    UnaryExpr(char op, std::unique_ptr<Expression> right, int line = -1, int column = -1);
    Value accept(ExprVisitor& val) override;
};

struct StringExpr : public Expression
{
    std::string value;

    explicit StringExpr(std::string val, int line = -1, int column = -1);
    Value accept(ExprVisitor& val) override;

    [[nodiscard]] std::string get_operator_value() const override;
    [[nodiscard]] char get_operator_char() const override;
};

struct BoolExpr : public Expression
{
    bool value;

    explicit BoolExpr(bool v, int line = -1, int column = -1);
    Value accept(ExprVisitor& val) override;
};

struct FunctionCallExpr : public Expression
{
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> arguments;

    FunctionCallExpr(std::unique_ptr<Expression> expr, std::vector<std::unique_ptr<Expression>> args, int line = -1, int column = -1);
    Value accept(ExprVisitor& val) override;
};

struct ArrayLiteralExpr : public Expression
{
    std::vector<std::unique_ptr<Expression>> elements;

    explicit ArrayLiteralExpr(std::vector<std::unique_ptr<Expression>> elems, int line = -1, int column = -1);
    Value accept(ExprVisitor& val) override;
};

struct IndexExpr : public Expression
{
    std::unique_ptr<Expression> array;
    std::unique_ptr<Expression> index;

    IndexExpr(std::unique_ptr<Expression> arr, std::unique_ptr<Expression> idx, int line = -1, int column = -1);
    Value accept(ExprVisitor& val) override;
};

struct MemberAccessExpr : public Expression
{
    std::unique_ptr<Expression> object;
    std::string member;

    MemberAccessExpr(std::unique_ptr<Expression> obj, std::string mem, int line = -1, int column = -1);
    Value accept(ExprVisitor& val) override;
};

struct DictionaryLiteralExpr : public Expression
{
    std::vector<std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>> entries;

    explicit DictionaryLiteralExpr(std::vector<std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>> entr, int line = -1, int column = -1);
    Value accept(ExprVisitor& val) override;
};

#endif //BERESTALANGUAGE_EXPRESSION_H
