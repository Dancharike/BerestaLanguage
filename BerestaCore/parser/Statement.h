//
// Created by Denis on 08.07.2025.
//

#ifndef BERESTALANGUAGE_STATEMENT_H
#define BERESTALANGUAGE_STATEMENT_H

#pragma once
#include "../value/Value.h"
#include <string>
#include <memory>
#include <vector>

struct Expression;

enum class StatementType
{
    ASSIGNMENT,
    EXPRESSION,
    IF,
    BLOCK
};

struct Statement
{
    StatementType type;
    explicit Statement(StatementType type) : type(type) {}
    virtual ~Statement() = default;
};

struct Assignment : public Statement
{
    std::string name;
    std::unique_ptr<Expression> value;

    Assignment(std::string name, std::unique_ptr<Expression> value);
};

struct ExpressionStatement : public Statement
{
    std::unique_ptr<Expression> expression;

    explicit ExpressionStatement(std::unique_ptr<Expression> expr);
};

struct IfStatement : public Statement
{
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> then_branch;
    std::unique_ptr<Statement> else_branch;

    IfStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> then_b, std::unique_ptr<Statement> else_b = nullptr);
};

struct BlockStatement : public Statement
{
    std::vector<std::unique_ptr<Statement>> statements;

    explicit BlockStatement(std::vector<std::unique_ptr<Statement>> stmt) : Statement(StatementType::BLOCK), statements(std::move(stmt)) {}
};

#endif //BERESTALANGUAGE_STATEMENT_H

