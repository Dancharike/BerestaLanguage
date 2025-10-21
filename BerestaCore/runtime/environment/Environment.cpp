//
// Created by Denis on 21.10.2025.
//

#include "Environment.h"

Environment::Environment(Diagnostics* diag, Environment* parent) : _diag(diag), _parent(parent), _scopes(1) {}

void Environment::push_scope() {_scopes.emplace_back();}
void Environment::pop_scope() {if(_scopes.size() > 1) {_scopes.pop_back();}}

void Environment::define(const std::string& name, const Value& val) {_scopes.back()[name] = val;}

void Environment::define_global(const std::string& name, const Value& val)
{
    if(_scopes.empty()) {_scopes.emplace_back();}
    _scopes.front()[name] = val;
}

bool Environment::assign(const std::string& name, const Value& v, const std::string& file, int line)
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

[[nodiscard]] Value Environment::get(const std::string& name, const std::string& file, int line) const
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

[[nodiscard]] bool Environment::exists(const std::string& name) const
{
    for(int i = static_cast<int>(_scopes.size()) - 1; i >= 0; --i)
    {
        if(_scopes[i].count(name)) {return true;}
    }
    return _parent ? _parent->exists(name) : false;
}