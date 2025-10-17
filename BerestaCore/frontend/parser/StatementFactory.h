//
// Created by Denis on 17.10.2025.
//

#ifndef BERESTALANGUAGE_STATEMENTFACTORY_H
#define BERESTALANGUAGE_STATEMENTFACTORY_H

#pragma once
#include <functional>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

class Statement;
class Expression;

class StatementFactory
{
    public:
        using Creator = std::function<std::unique_ptr<Statement>(std::vector<std::unique_ptr<Statement>>&&, std::vector<std::unique_ptr<Expression>>&&)>;


    static StatementFactory& instance();
        void register_type(const std::string& name, Creator func);
        std::unique_ptr<Statement> create(const std::string& name, std::vector<std::unique_ptr<Statement>>&& stmts = {}, std::vector<std::unique_ptr<Expression>>&& exprs = {}) const;

    private:
        std::unordered_map<std::string, Creator> _registry;
};


#endif //BERESTALANGUAGE_STATEMENTFACTORY_H
