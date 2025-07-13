//
// Created by Denis on 13.07.2025.
//

#include "Statement.h"
#include "Expression.h"

Assignment::Assignment(std::string name, std::unique_ptr<Expression> value)
    : Statement(StatementType::ASSIGNMENT), name(std::move(name)), value(std::move(value)) {}

ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expr)
    : Statement(StatementType::EXPRESSION), expression(std::move(expr)) {}

IfStatement::IfStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> then_b, std::unique_ptr<Statement> else_b)
    : Statement(StatementType::IF), condition(std::move(cond)), then_branch(std::move(then_b)), else_branch(std::move(else_b)) {}