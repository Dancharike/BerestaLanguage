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
    ASSIGNMENT_STATEMENT,
    EXPRESSION,
    IF,
    WHILE,
    REPEAT,
    FOR,
    BLOCK,
    FUNCTION,
    RETURN
};

enum class FunctionVisibility
{
    PUBLIC,
    PRIVATE
};

struct Statement
{
    StatementType type;
    explicit Statement(StatementType type) : type(type) {}
    virtual ~Statement() = default;
};

struct Assignment : public Statement
{
    bool is_let;
    std::string name;
    std::unique_ptr<Expression> value;

    Assignment(bool is_let, std::string name, std::unique_ptr<Expression> value);
};

struct AssignmentStatement : public Statement
{
    std::unique_ptr<Assignment> assignment;

    explicit AssignmentStatement(std::unique_ptr<Assignment> assign);
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

struct WhileStatement : public Statement
{
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;

    WhileStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> body);
};

struct RepeatStatement : public Statement
{
    std::unique_ptr<Expression> count;
    std::unique_ptr<Statement> body;

    RepeatStatement(std::unique_ptr<Expression> count, std::unique_ptr<Statement> body);
};

struct ForStatement : public Statement
{
    std::unique_ptr<Statement> initializer;
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> increment;
    std::unique_ptr<Statement> body;

    ForStatement(std::unique_ptr<Statement> init, std::unique_ptr<Expression> cond, std::unique_ptr<Statement> inc, std::unique_ptr<Statement> body);
};

struct BlockStatement : public Statement
{
    std::vector<std::unique_ptr<Statement>> statements;

    explicit BlockStatement(std::vector<std::unique_ptr<Statement>> stmt) : Statement(StatementType::BLOCK), statements(std::move(stmt)) {}
};

struct FunctionStatement : public Statement
{
    FunctionVisibility visibility;
    std::string name;
    std::vector<std::string> parameters;
    std::unique_ptr<Statement> body;

    FunctionStatement(FunctionVisibility vis, std::string name, std::vector<std::string> params, std::unique_ptr<Statement> body);
};

struct ReturnStatement : public Statement
{
    std::unique_ptr<Expression> value;

    explicit ReturnStatement(std::unique_ptr<Expression> val);
};

#endif //BERESTALANGUAGE_STATEMENT_H

