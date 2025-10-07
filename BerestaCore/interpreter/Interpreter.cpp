//
// Created by Denis on 08.07.2025.
//

#include "Interpreter.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"

Interpreter::Interpreter(Environment& env, FunctionIndex& index, Diagnostics& diag) : BaseContext(diag), _env(env), _index(index) {}

void Interpreter::register_file(const std::string& filename, const std::string& code)
{
    Lexer lexer(code);
    auto tokens = lexer.tokenize();

    Parser parser(tokens, _env, _index, filename, _diag);
    auto statements = parser.parse();

    FileUnit unit;
    unit.ast = std::move(statements);
    _files[filename] = std::move(unit);

    index_functions(filename, _files[filename].ast);
}

void Interpreter::index_functions(const std::string& filename, const std::vector<std::unique_ptr<Statement>>& statements)
{
    _index.reindex_file(filename, statements);
}

void Interpreter::execute_file(const std::string& filename)
{
    set_current_file(filename);

    auto it = _files.find(filename);
    if(it == _files.end()) {_diag.error("File not registered: " + filename, current_file()); return;}

    interpret(it->second.ast, filename);

    if(_diag.has_error())
    {
        _diag.print_all();
    }
}

Value Interpreter::interpret(const std::vector<std::unique_ptr<Statement>>& statements, const std::string& current_file)
{
    Evaluator evaluator(_env, _index, current_file, _diag);

    Value last_value;
    for(const auto& stmt : statements)
    {
        if(stmt->type == StatementType::FUNCTION) {continue;}
        last_value = evaluator.eval_statement(stmt.get());
    }

    return last_value;
}

void Interpreter::run_project(const std::string& entry_file)
{
    execute_file(entry_file);
}
