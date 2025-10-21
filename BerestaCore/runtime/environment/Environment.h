//
// Created by Denis on 04.10.2025.
//

#ifndef BERESTALANGUAGE_ENVIRONMENT_H
#define BERESTALANGUAGE_ENVIRONMENT_H

#pragma once
#include "runtime/value/Value.h"
#include "frontend/diagnostics/Diagnostics.h"
#include <string>
#include <unordered_map>
#include <vector>

class Environment
{
    public:
        explicit Environment(Diagnostics* diag, Environment* parent = nullptr);

        void push_scope();
        void pop_scope();
        void define(const std::string& name, const Value& val);
        void define_global(const std::string& name, const Value& val);

        bool assign(const std::string& name, const Value& v, const std::string& file = "", int line = -1);
        [[nodiscard]] Value get(const std::string& name, const std::string& file = "", int line = -1) const;
        [[nodiscard]] bool exists(const std::string& name) const;

    private:
        std::vector<std::unordered_map<std::string, Value>> _scopes;
        Environment* _parent;
        Diagnostics* _diag;
};


#endif //BERESTALANGUAGE_ENVIRONMENT_H
