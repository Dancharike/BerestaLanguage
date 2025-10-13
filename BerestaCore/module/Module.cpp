//
// Created by Denis on 12.10.2025.
//

#include "Module.h"

Module::Module(std::string filename, Environment* env, FunctionIndex& index, Diagnostics& diag)
    : BaseContext(diag, filename), _filename(std::move(filename)), _env(env), _index(index) {}

void Module::set_ast(std::vector<std::unique_ptr<Statement>> stmts) {_ast = std::move(stmts);}

const std::vector<std::unique_ptr<Statement>>& Module::get_ast() const {return _ast;}

Environment& Module::environment() {return *_env;}

FunctionIndex& Module::index() {return _index;}
