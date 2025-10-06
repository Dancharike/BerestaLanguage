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
#include <unordered_map>

struct Expression;

enum class StatementType
{
    ASSIGNMENT,
    ASSIGNMENT_STATEMENT,
    INDEX_ASSIGNMENT,
    EXPRESSION,
    IF,
    WHILE,
    REPEAT,
    FOR,
    FOREACH,
    BLOCK,
    FUNCTION,
    ENUM,
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
    int line;
    int column;

    explicit Statement(StatementType type, int line = -1, int column = -1);
    virtual ~Statement() = default;
};

struct Assignment : public Statement
{
    bool is_let;
    std::string name;
    std::unique_ptr<Expression> value;

    Assignment(bool is_let, std::string name, std::unique_ptr<Expression> value, int line = -1, int column = -1);
};

struct AssignmentStatement : public Statement
{
    std::unique_ptr<Assignment> assignment;

    explicit AssignmentStatement(std::unique_ptr<Assignment> assign, int line = -1, int column = -1);
};

struct ExpressionStatement : public Statement
{
    std::unique_ptr<Expression> expression;

    explicit ExpressionStatement(std::unique_ptr<Expression> expr, int line = -1, int column = -1);
};

struct IfStatement : public Statement
{
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> then_branch;
    std::unique_ptr<Statement> else_branch;

    IfStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> then_b, std::unique_ptr<Statement> else_b = nullptr, int line = -1, int column = -1);
};

struct WhileStatement : public Statement
{
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;

    WhileStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> body, int line = -1, int column = -1);
};

struct RepeatStatement : public Statement
{
    std::unique_ptr<Expression> count;
    std::unique_ptr<Statement> body;

    RepeatStatement(std::unique_ptr<Expression> count, std::unique_ptr<Statement> body, int line = -1, int column = -1);
};

struct ForStatement : public Statement
{
    std::unique_ptr<Statement> initializer;
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> increment;
    std::unique_ptr<Statement> body;

    ForStatement(std::unique_ptr<Statement> init, std::unique_ptr<Expression> cond, std::unique_ptr<Statement> inc, std::unique_ptr<Statement> body, int line = -1, int column = -1);
};

struct ForeachStatement : public Statement
{
    std::string var_name;
    std::unique_ptr<Expression> iterable;
    std::unique_ptr<Statement> body;

    ForeachStatement(std::string var, std::unique_ptr<Expression> iter, std::unique_ptr<Statement> body, int line = -1, int column = -1);
};

struct BlockStatement : public Statement
{
    std::vector<std::unique_ptr<Statement>> statements;

    explicit BlockStatement(std::vector<std::unique_ptr<Statement>> stmt, int line = -1, int column = -1);
};

struct FunctionStatement : public Statement
{
    FunctionVisibility visibility;
    std::string name;
    std::vector<std::string> parameters;
    std::unique_ptr<Statement> body;

    FunctionStatement(FunctionVisibility vis, std::string name, std::vector<std::string> params, std::unique_ptr<Statement> body, int line = -1, int column = -1);
};

struct ReturnStatement : public Statement
{
    std::unique_ptr<Expression> value;

    explicit ReturnStatement(std::unique_ptr<Expression> val, int line = -1, int column = -1);
};

struct IndexAssignment : public Statement
{
    std::unique_ptr<Expression> target;
    std::unique_ptr<Expression> value;

    IndexAssignment(std::unique_ptr<Expression> t, std::unique_ptr<Expression> v, int line = -1, int column = -1);
};

struct EnumStatement : Statement
{
    std::string name;
    std::unordered_map<std::string, int> members;

    EnumStatement(std::string  n, std::unordered_map<std::string, int> m, int line = -1, int column = -1);
};

#endif //BERESTALANGUAGE_STATEMENT_H

