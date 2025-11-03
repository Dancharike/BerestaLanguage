//
// Created by Denis on 09.10.2025.
//

#include "BuiltinRegistry.h"
#include "runtime/environment/Environment.h"
#include <utility>

void register_builtin_console_print();
void register_builtin_math_core();
void register_builtin_math_trig();
void register_builtin_random();
void register_builtin_matrix_core();
void register_builtin_array();
void register_builtin_dictionary();

BuiltinRegistry& BuiltinRegistry::instance()
{
    static BuiltinRegistry inst;
    return inst;
}

void BuiltinRegistry::register_builtin(std::unique_ptr<IBuiltinFunction> func)
{
    if(!func) {return;}
    _builtins[func->name()] = std::move(func);
}

IBuiltinFunction* BuiltinRegistry::get(const std::string& name)
{
    auto it = _builtins.find(name);
    return (it == _builtins.end()) ? nullptr : it->second.get();
}

void BuiltinRegistry::clear()
{
    _builtins.clear();
}

static Environment* g_active_env = nullptr;
void set_active_environment(Environment* env) {g_active_env = env;}
Environment* get_active_environment() {return g_active_env;}

void register_default_builtins()
{
    register_builtin_console_print();
    register_builtin_math_core();
    register_builtin_math_trig();
    register_builtin_random();
    register_builtin_matrix_core();
    register_builtin_array();
    register_builtin_dictionary();
}
