//
// Created by Denis on 13.07.2025.
//

#include "Statement.h"
#include "Expression.h"
#include "Visitors.h"
#include "frontend/parser/StatementFactory.h"

Statement::Statement(StatementType type, int line, int column)
    : type(type), line(line), column(column) {}

Assignment::Assignment(bool is_let, std::string name, std::unique_ptr<Expression> value, int line, int column)
    : Statement(StatementType::ASSIGNMENT, line, column), is_let(is_let), name(std::move(name)), value(std::move(value)) {}

AssignmentStatement::AssignmentStatement(std::unique_ptr<Assignment> assign, int line, int column)
    : Statement(StatementType::ASSIGNMENT_STATEMENT, line, column), assignment(std::move(assign)) {}

ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expr, int line, int column)
    : Statement(StatementType::EXPRESSION, line, column), expression(std::move(expr)) {}

IfStatement::IfStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> then_b, std::unique_ptr<Statement> else_b, int line, int column)
    : Statement(StatementType::IF, line, column), condition(std::move(cond)), then_branch(std::move(then_b)), else_branch(std::move(else_b)) {}

WhileStatement::WhileStatement(std::unique_ptr<Expression> cond, std::unique_ptr<Statement> body, int line, int column)
    : Statement(StatementType::WHILE, line, column), condition(std::move(cond)), body(std::move(body)) {}

RepeatStatement::RepeatStatement(std::unique_ptr<Expression> count, std::unique_ptr<Statement> body, int line, int column)
    : Statement(StatementType::REPEAT, line, column), count(std::move(count)), body(std::move(body)) {}

ForStatement::ForStatement(std::unique_ptr<Statement> init, std::unique_ptr<Expression> cond,std::unique_ptr<Statement> inc, std::unique_ptr<Statement> body, int line, int column)
    : Statement(StatementType::FOR, line, column), initializer(std::move(init)), condition(std::move(cond)), increment(std::move(inc)), body(std::move(body)) {}

ForeachStatement::ForeachStatement(std::string var, std::unique_ptr<Expression> iter, std::unique_ptr<Statement> body, int line, int column)
    : Statement(StatementType::FOREACH, line, column), var_name(std::move(var)), iterable(std::move(iter)), body(std::move(body)) {}

FunctionStatement::FunctionStatement(FunctionVisibility vis,std::string name, std::vector<std::string> params,std::unique_ptr<Statement> body, int line, int column)
    : Statement(StatementType::FUNCTION, line, column), visibility(vis), name(std::move(name)), parameters(std::move(params)), body(std::move(body)) {}

BlockStatement::BlockStatement(std::vector<std::unique_ptr<Statement>> stmt, int line, int column)
    : Statement(StatementType::BLOCK, line, column), statements(std::move(stmt)) {}

ReturnStatement::ReturnStatement(std::unique_ptr<Expression> val, int line, int column)
    : Statement(StatementType::RETURN, line, column), value(std::move(val)) {}

IndexAssignment::IndexAssignment(std::unique_ptr<Expression> t, std::unique_ptr<Expression> v, int line, int column)
    : Statement(StatementType::INDEX_ASSIGNMENT, line, column), target(std::move(t)), value(std::move(v)) {}

EnumStatement::EnumStatement(std::string n, std::unordered_map<std::string, int> m, int line, int column)
    : Statement(StatementType::ENUM, line, column), name(std::move(n)), members(std::move(m)) {}

MacrosStatement::MacrosStatement(std::string name, std::unique_ptr<Expression> val, int line, int column)
    : Statement(StatementType::MACROS, line, column), name(std::move(name)), value(std::move(val)) {}

Value Assignment::accept(StmtVisitor& val)          {return val.visit_assignment(*this);}
Value AssignmentStatement::accept(StmtVisitor& val) {return val.visit_assignment_statement(*this);}
Value ExpressionStatement::accept(StmtVisitor& val) {return val.visit_expr_stmt(*this);}
Value IfStatement::accept(StmtVisitor& val)         {return val.visit_if(*this);}
Value WhileStatement::accept(StmtVisitor& val)      {return val.visit_while(*this);}
Value RepeatStatement::accept(StmtVisitor& val)     {return val.visit_repeat(*this);}
Value ForStatement::accept(StmtVisitor& val)        {return val.visit_for(*this);}
Value ForeachStatement::accept(StmtVisitor& val)    {return val.visit_foreach(*this);}
Value BlockStatement::accept(StmtVisitor& val)      {return val.visit_block(*this);}
Value FunctionStatement::accept(StmtVisitor& val)   {return val.visit_function(*this);}
Value ReturnStatement::accept(StmtVisitor& val)     {return val.visit_return(*this);}
Value IndexAssignment::accept(StmtVisitor& val)     {return val.visit_index_assignment(*this);}
Value EnumStatement::accept(StmtVisitor& val)       {return val.visit_enum(*this);}
Value MacrosStatement::accept(StmtVisitor& val)     {return val.visit_macros(*this);}

// этот код нигде не используется, но нужен при компиляции, ещё до момента, как файл .beresta начал читаться, чтобы в нужный момент фабрика получила ссылку на способ сборки объекта
[[maybe_unused]] static bool reg_expr_stmt = []
{
    StatementFactory::instance().register_type("expr_stmt", [](std::vector<std::unique_ptr<Statement>>&&, std::vector<std::unique_ptr<Expression>>&& exprs) -> std::unique_ptr<Statement>
    {
        if(exprs.empty()) {return nullptr;}
        return std::make_unique<ExpressionStatement>(std::move(exprs[0]));
    });
    return true;
}();

[[maybe_unused]] static bool reg_if_stmt = []
{
    StatementFactory::instance().register_type("if", [](std::vector<std::unique_ptr<Statement>>&& stmts, std::vector<std::unique_ptr<Expression>>&& exprs) -> std::unique_ptr<Statement>
    {
        if(exprs.empty() || stmts.empty()) {return nullptr;}
        auto cond = std::move(exprs[0]);
        auto then_branch = std::move(stmts[0]);
        std::unique_ptr<Statement> else_branch = (stmts.size() > 1) ? std::move(stmts[1]) : nullptr;
        return std::make_unique<IfStatement>(std::move(cond), std::move(then_branch), std::move(else_branch));
    });
    return true;
}();

[[maybe_unused]] static bool reg_while_stmt = []
{
    StatementFactory::instance().register_type("while", [](std::vector<std::unique_ptr<Statement>>&& stmts, std::vector<std::unique_ptr<Expression>>&& exprs) -> std::unique_ptr<Statement>
    {
        if(exprs.empty() || stmts.empty()) {return nullptr;}
        return std::make_unique<WhileStatement>(std::move(exprs[0]), std::move(stmts[0]));
    });
    return true;
}();

[[maybe_unused]] static bool reg_repeat_stmt = []
{
    StatementFactory::instance().register_type("repeat", [](std::vector<std::unique_ptr<Statement>>&& stmts, std::vector<std::unique_ptr<Expression>>&& exprs) -> std::unique_ptr<Statement>
    {
        if(exprs.empty() || stmts.empty()) {return nullptr;}
        return std::make_unique<RepeatStatement>(std::move(exprs[0]), std::move(stmts[0]));
    });
    return true;
}();

[[maybe_unused]] static bool reg_for_stmt = []
{
    StatementFactory::instance().register_type("for", [](std::vector<std::unique_ptr<Statement>>&& stmts, std::vector<std::unique_ptr<Expression>>&& exprs) -> std::unique_ptr<Statement>
    {
        if(exprs.empty()) {return nullptr;}
        auto init = (stmts.size() > 0) ? std::move(stmts[0]) : nullptr;
        auto inc  = (stmts.size() > 1) ? std::move(stmts[1]) : nullptr;
        auto body = (stmts.size() > 2) ? std::move(stmts[2]) : nullptr;
        auto cond = std::move(exprs[0]);
        return std::make_unique<ForStatement>(std::move(init), std::move(cond), std::move(inc), std::move(body));
    });
    return true;
}();

[[maybe_unused]] static bool reg_foreach_stmt = []
{
    StatementFactory::instance().register_type("foreach", [](std::vector<std::unique_ptr<Statement>>&& stmts, std::vector<std::unique_ptr<Expression>>&& exprs) -> std::unique_ptr<Statement>
    {
        if(exprs.empty() || stmts.empty()) {return nullptr;}
        return std::make_unique<ForeachStatement>("<var>", std::move(exprs[0]), std::move(stmts[0]));
    });
    return true;
}();

[[maybe_unused]] static bool reg_block_stmt = []
{
    StatementFactory::instance().register_type("block", [](std::vector<std::unique_ptr<Statement>>&& stmts, std::vector<std::unique_ptr<Expression>>&&) -> std::unique_ptr<Statement>
    {
        return std::make_unique<BlockStatement>(std::move(stmts));
    });
    return true;
}();

[[maybe_unused]] static bool reg_function_stmt = []
{
    StatementFactory::instance().register_type("function", [](std::vector<std::unique_ptr<Statement>>&& stmts, std::vector<std::unique_ptr<Expression>>&&) -> std::unique_ptr<Statement>
    {
        if(stmts.empty()) {return nullptr;}
        return std::make_unique<FunctionStatement>(FunctionVisibility::PUBLIC, "<anon>", std::vector<std::string>{}, std::move(stmts[0]));
    });
    return true;
}();

[[maybe_unused]] static bool reg_return_stmt = []
{
    StatementFactory::instance().register_type("return", [](std::vector<std::unique_ptr<Statement>>&&, std::vector<std::unique_ptr<Expression>>&& exprs) -> std::unique_ptr<Statement>
    {
        std::unique_ptr<Expression> val = exprs.empty() ? nullptr : std::move(exprs[0]);
        return std::make_unique<ReturnStatement>(std::move(val));
    });
    return true;
}();

[[maybe_unused]] static bool reg_index_assign_stmt = []
{
    StatementFactory::instance().register_type("index_assign", [](std::vector<std::unique_ptr<Statement>>&&, std::vector<std::unique_ptr<Expression>>&& exprs) -> std::unique_ptr<Statement>
    {
        if(exprs.size() < 2) {return nullptr;}
        return std::make_unique<IndexAssignment>(std::move(exprs[0]), std::move(exprs[1]));
    });
    return true;
}();

[[maybe_unused]] static bool reg_enum_stmt = []
{
    StatementFactory::instance().register_type("enum", [](std::vector<std::unique_ptr<Statement>>&&, std::vector<std::unique_ptr<Expression>>&&) -> std::unique_ptr<Statement>
    {
        return std::make_unique<EnumStatement>("<unnamed>", std::unordered_map<std::string, int>{});
    });
    return true;
}();
