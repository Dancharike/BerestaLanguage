//
// Created by Denis on 08.07.2025.
//

#include "Interpreter.h"
#include "../frontend/lexer/Lexer.h"
#include "../runtime/builtin/core/BuiltinRegistry.h"
#include "../runtime/evaluator/Evaluator.h"

Interpreter::Interpreter(Environment& env, FunctionIndex& index, Diagnostics& diag) : BaseContext(diag), _env(env), _index(index), _modules(_diag)
{
    register_default_builtins();
}

void Interpreter::register_file(const std::string& filename, const std::string& code)
{
    Lexer lexer(code);
    auto tokens = lexer.tokenize();

    StatementParser parser(tokens, filename, _diag);
    auto statements = parser.parse();

    Module& module = _modules.register_module(filename, _env, _index);
    module.set_ast(std::move(statements));

    _index.reindex_file(filename, module.get_ast());
}

void Interpreter::run_project(const std::string& entry_file)
{
    Module* entry = _modules.get_module(entry_file);
    if(!entry) {_diag.error("File not registered: " + entry_file, entry_file); return;}

    auto files = _modules.list_filenames();
    for(const auto& filename : files)
    {
        if(filename == entry_file) {continue;}

        Module* mod = _modules.get_module(filename);
        if(!mod) {continue;}

        set_current_file(filename);

        Environment& env = mod->environment();
        env.push_scope();

        Evaluator eval(env, mod->index(), filename, _diag);
        for(const auto& stmt : mod->get_ast())
        {
            if(stmt->type != StatementType::FUNCTION) {eval.eval_statement(stmt.get());}
        }

        env.pop_scope();
    }


    set_current_file(entry_file);

    Environment& env = entry->environment();
    env.push_scope();

    Evaluator eval(env, entry->index(), entry_file, _diag);
    for(const auto& stmt : entry->get_ast())
    {
        if(stmt->type != StatementType::FUNCTION) {eval.eval_statement(stmt.get());}
    }

    env.pop_scope();

    if(_diag.has_error()) {_diag.print_all();}
}
