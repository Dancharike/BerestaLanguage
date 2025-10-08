//
// Created by Denis on 13.07.2025.
//

#include "Statement.h"
#include "Expression.h"
#include "Visitors.h"

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
