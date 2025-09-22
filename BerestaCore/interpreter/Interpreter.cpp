//
// Created by Denis on 08.07.2025.
//

#include "Interpreter.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include <iostream>
#include <vector>

void Interpreter::index_functions(const std::string& filename, const std::vector<std::unique_ptr<Statement>>& statements)
{
    auto& unit = files[filename];
    unit.private_functions.clear();

    for(const auto& stmt : statements)
    {
        if(stmt->type == StatementType::FUNCTION)
        {
            auto* fn = dynamic_cast<FunctionStatement*>(stmt.get());
            if(fn->visibility == FunctionVisibility::PUBLIC)
            {
                public_functions[fn->name] = {fn, filename};
            }
            else
            {
                unit.private_functions[fn->name] = fn;
            }
        }
    }

    private_functions_by_file[filename] = unit.private_functions;
}

void Interpreter::register_file(const std::string& filename, const std::string& code)
{
    Lexer lexer(code);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto statements = parser.parse();

    FileUnit unit;
    unit.ast = std::move(statements);
    files[filename] = std::move(unit);

    index_functions(filename, files[filename].ast);
}

void Interpreter::execute_file(const std::string& filename)
{
    auto it = files.find(filename);
    if(it == files.end()) { std::cerr << "[ERROR] File not registered: " << filename << "\n"; return; }

    set_public_functions(&public_functions);
    set_private_functions(&private_functions_by_file);
    set_current_file(filename);

    interpret(it->second.ast);
}

std::pair<std::string, Value> Interpreter::interpret(const std::vector<std::unique_ptr<Statement>>& statements)
{
    std::string last_name;
    Value last_value;

    for(const auto& stmt : statements)
    {
        if(stmt->type == StatementType::ASSIGNMENT)
        {
            auto* assign = dynamic_cast<Assignment*>(stmt.get());
            if(!assign) {std::cerr << "[ERROR] Invalid statement type in interpreter\n"; continue;}
            Value result = evaluate(assign->value.get(), variables);
            variables[assign->name] = result;
            last_name = assign->name;
            last_value = result;
        }

        if(stmt->type == StatementType::ASSIGNMENT_STATEMENT)
        {
            auto* assign_stmt = dynamic_cast<AssignmentStatement*>(stmt.get());
            if(!assign_stmt) {std::cerr << "[ERROR] Invalid assignment statement cast\n"; continue;}
            Value result = evaluate(assign_stmt->assignment->value.get(), variables);
            variables[assign_stmt->assignment->name] = result;
            last_name = assign_stmt->assignment->name;
            last_value = result;
        }

        if(stmt->type == StatementType::EXPRESSION)
        {
            auto* expr_stmt = dynamic_cast<ExpressionStatement*>(stmt.get());
            if(expr_stmt) {evaluate(expr_stmt->expression.get(), variables);}
        }

        if(stmt->type == StatementType::BLOCK)
        {
            auto* block = dynamic_cast<BlockStatement*>(stmt.get());
            if(!block) {std::cerr << "[ERROR] Invalid block cast\n"; continue;}
            interpret(block->statements);
        }

        if(stmt->type == StatementType::IF) {evaluate(stmt.get(), variables);}
        if(stmt->type == StatementType::WHILE) {evaluate(stmt.get(), variables);}
        if(stmt->type == StatementType::REPEAT) {evaluate(stmt.get(), variables);}
        if(stmt->type == StatementType::FOR) {evaluate(stmt.get(), variables);}
        if(stmt->type == StatementType::FUNCTION) {}
    }

    return {last_name, last_value};
}

void Interpreter::run_project(const std::string& entry_file)
{
    execute_file(entry_file);
}
