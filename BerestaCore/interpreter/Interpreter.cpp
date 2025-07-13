//
// Created by Denis on 08.07.2025.
//

#include "Interpreter.h"
#include "../parser/Evaluator.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include <iostream>
#include <vector>

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

        if(stmt->type == StatementType::IF)
        {
            evaluate(stmt.get(), variables);
        }
    }

    return {last_name, last_value};
}

void Interpreter::print_variables() const
{
    for(const auto& [name, value] : variables)
    {
        std::cout << name << " = " << value.to_string() << std::endl;
    }
}

void Interpreter::run(const std::string& code)
{
    Lexer lexer(code);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto statements = parser.parse();

    interpret(statements);

    /*
    auto [name, value] = interpret(statements);
    if (!name.empty())
    {
        std::cout << " " << name << " = " << value.to_string() << std::endl;
    }
     */
}