//
// Created by Denis on 16.10.2025.
//

#ifndef BERESTALANGUAGE_EXPRESSIONFACTORY_H
#define BERESTALANGUAGE_EXPRESSIONFACTORY_H

#pragma once
#include <functional>
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

class Expression;

class ExpressionFactory
{
    public:
        using Creator = std::function<std::unique_ptr<Expression>(std::vector<std::unique_ptr<Expression>>&& args)>;

        static ExpressionFactory& instance();
        void register_type(const std::string& name, Creator func);
        std::unique_ptr<Expression> create(const std::string& name, std::vector<std::unique_ptr<Expression>>&& args) const;

    private:
        std::unordered_map<std::string, Creator> _registry;
};


#endif //BERESTALANGUAGE_EXPRESSIONFACTORY_H
