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
#include "../diagnostics/Diagnostics.h"

class Environment
{
    public:
        explicit Environment(Diagnostics* diag, Environment* parent = nullptr) : _diag(diag), _parent(parent), _scopes(1) {}

        void push_scope() {_scopes.emplace_back();}
        void pop_scope() {if(_scopes.size() > 1) {_scopes.pop_back();}}

        void define(const std::string& name, const Value& val) {_scopes.back()[name] = val;}

        bool assign(const std::string& name, const Value& v, const std::string& file = "", int line = -1)
        {
            for(int i = static_cast<int>(_scopes.size()) - 1; i >= 0; --i)
            {
                auto it = _scopes[i].find(name);
                if(it != _scopes[i].end()) {it->second = v; return true;}
            }
            if(_parent) {return _parent->assign(name, v, file, line);}
            _scopes.front()[name] = v;
            return true;
        }

        [[nodiscard]] Value get(const std::string& name, const std::string& file = "", int line = -1) const
        {
            for(int i = static_cast<int>(_scopes.size()) - 1; i >= 0; --i)
            {
                auto it = _scopes[i].find(name);
                if(it != _scopes[i].end()) {return it->second;}
            }
            if(_parent) {return _parent->get(name, file, line);}
            if(_diag) {_diag->error("Variable not found: " + name, file, line);}
            return {};
        }

    private:
        std::vector<std::unordered_map<std::string, Value>> _scopes;
        Environment* _parent;
        Diagnostics* _diag;
};


#endif //BERESTALANGUAGE_ENVIRONMENT_H
