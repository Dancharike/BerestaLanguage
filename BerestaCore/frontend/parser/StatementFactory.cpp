//
// Created by Denis on 17.10.2025.
//

#include "StatementFactory.h"
#include "Statement.h"

StatementFactory& StatementFactory::instance()
{
    static StatementFactory inst;
    return inst;
}

void StatementFactory::register_type(const std::string& name, StatementFactory::Creator func)
{
    _registry[name] = std::move(func);
}

std::unique_ptr<Statement> StatementFactory::create(const std::string& name, std::vector<std::unique_ptr<Statement>>&& stmts, std::vector<std::unique_ptr<Expression>>&& exprs) const
{
    auto it = _registry.find(name);
    if(it == _registry.end()) {return nullptr;}
    return it->second(std::move(stmts), std::move(exprs));
}
