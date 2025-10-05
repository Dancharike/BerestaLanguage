//
// Created by Denis on 04.10.2025.
//

#ifndef BERESTALANGUAGE_ENVIRONMENT_H
#define BERESTALANGUAGE_ENVIRONMENT_H

#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "../value/Value.h"

class Environment
{
    public:
        Environment() : _scopes(1) {}

        void push_scope() {_scopes.emplace_back();}
        void pop_scope() {if(_scopes.size() > 1) {_scopes.pop_back();}}

        void define(const std::string& name, const Value& val) {_scopes.back()[name] = val;}

        bool assign(const std::string& name, const Value& v)
        {
            for(int i = static_cast<int>(_scopes.size()) - 1; i >= 0; --i)
            {
                auto it = _scopes[i].find(name);
                if(it != _scopes[i].end()) {it->second = v; return true;}
            }
            _scopes.front()[name] = v;
            return true;
        }

        [[nodiscard]] Value get(const std::string& name) const
        {
            for(int i = static_cast<int>(_scopes.size()) - 1; i >= 0; --i)
            {
                auto it = _scopes[i].find(name);
                if(it != _scopes[i].end()) {return it->second;}
            }
            std::cerr << "[ERROR] Variable not found: " << name << "\n";
            return {};
        }

    private:
        std::vector<std::unordered_map<std::string, Value>> _scopes;
};


#endif //BERESTALANGUAGE_ENVIRONMENT_H
