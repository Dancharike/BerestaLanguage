//
// Created by Denis on 16.10.2025.
//

#include "ExpressionFactory.h"
#include "Expression.h"

ExpressionFactory& ExpressionFactory::instance()
{
    static ExpressionFactory inst;
    return inst;
}

void ExpressionFactory::register_type(const std::string& name, ExpressionFactory::Creator func)
{
    _registry[name] = std::move(func);
}

std::unique_ptr<Expression> ExpressionFactory::create(const std::string& name, std::vector<std::unique_ptr<Expression>>&& args) const
{
    auto it = _registry.find(name);
    if(it == _registry.end()) {return nullptr;}
    return it->second(std::move(args));
}
