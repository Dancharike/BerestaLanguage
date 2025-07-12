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
#include <vector>

enum class StatementType
{
    ASSIGNMENT,
    EXPRESSION,
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

    Assignment(std::string name, std::unique_ptr<Expression> value): Statement(StatementType::ASSIGNMENT), name(std::move(name)), value(std::move(value)) {}
};

struct ExpressionStatement : public Statement
{
    std::unique_ptr<Expression> expression;

    explicit ExpressionStatement(std::unique_ptr<Expression> expr) : Statement(StatementType::EXPRESSION), expression(std::move(expr)) {}
};

struct BlockStatement : public Statement
{
    std::vector<std::unique_ptr<Statement>> statements;

    explicit BlockStatement(std::vector<std::unique_ptr<Statement>> stmt) : Statement(StatementType::BLOCK), statements(std::move(stmt)) {}
};

#endif //BERESTALANGUAGE_STATEMENT_H
