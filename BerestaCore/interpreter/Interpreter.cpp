//
// Created by Denis on 08.07.2025.
//

#include "Interpreter.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include <iostream>

Interpreter::Interpreter() = default;

void Interpreter::register_file(const std::string& filename, const std::string& code)
{
    Lexer lexer(code);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto statements = parser.parse();

    FileUnit unit;
    unit.ast = std::move(statements);
    _files[filename] = std::move(unit);

    index_functions(filename, _files[filename].ast);
}

void Interpreter::index_functions(const std::string& filename, const std::vector<std::unique_ptr<Statement>>& statements)
{
    _functions.reindex_file(filename, statements);
}

void Interpreter::execute_file(const std::string& filename)
{
    auto it = _files.find(filename);
    if(it == _files.end()) {std::cerr << "[ERROR] File not registered: " << filename << "\n"; return;}

    interpret(it->second.ast, filename);
}

Value Interpreter::interpret(const std::vector<std::unique_ptr<Statement>>& statements, const std::string& current_file)
{
    Evaluator evaluator(_env, _functions, current_file);

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
