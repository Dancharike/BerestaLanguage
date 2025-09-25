//
// Created by Denis on 13.07.2025.
//

#include "Statement.h"
#include "Expression.h"

Assignment::Assignment(bool is_let, std::string name, std::unique_ptr<Expression> value)
    : Statement(StatementType::ASSIGNMENT), is_let(is_let), name(std::move(name)), value(std::move(value)) {}

AssignmentStatement::AssignmentStatement(std::unique_ptr<Assignment> assign)
    : Statement(StatementType::ASSIGNMENT_STATEMENT), assignment(std::move(assign)) {}

ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expr)
    : Statement(StatementType::EXPRESSION), expression(std::move(expr)) {}

IfStatement::IfStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> then_b, std::unique_ptr<Statement> else_b)
    : Statement(StatementType::IF), condition(std::move(cond)), then_branch(std::move(then_b)), else_branch(std::move(else_b)) {}

WhileStatement::WhileStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> body)
    : Statement(StatementType::WHILE), condition(std::move(cond)), body(std::move(body)) {}

RepeatStatement::RepeatStatement(std::unique_ptr<Expression> count, std::unique_ptr<Statement> body)
    : Statement(StatementType::REPEAT), count(std::move(count)), body(std::move(body)) {}

ForStatement::ForStatement(std::unique_ptr<Statement> init, std::unique_ptr<Expression> cond,std::unique_ptr<Statement> inc, std::unique_ptr<Statement> body)
    : Statement(StatementType::FOR), initializer(std::move(init)), condition(std::move(cond)), increment(std::move(inc)), body(std::move(body)) {}

FunctionStatement::FunctionStatement(FunctionVisibility vis,std::string name, std::vector<std::string> params,std::unique_ptr<Statement> body)
    : Statement(StatementType::FUNCTION), visibility(vis), name(std::move(name)), parameters(std::move(params)), body(std::move(body)) {}

ReturnStatement::ReturnStatement(std::unique_ptr<Expression> val)
    : Statement(StatementType::RETURN), value(std::move(val)) {}

IndexAssignment::IndexAssignment(std::unique_ptr<Expression> t, std::unique_ptr<Expression> v)
    : Statement(StatementType::INDEX_ASSIGNMENT), target(std::move(t)), value(std::move(v)) {}

EnumStatement::EnumStatement(std::string n, std::unordered_map<std::string, int> m)
    : Statement(StatementType::ENUM), name(std::move(n)), members(std::move(m)) {}