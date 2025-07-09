//
// Created by Denis on 08.07.2025.
//

#include "Interpreter.h"
#include <iostream>
#include <vector>

void Interpreter::interpret(const std::vector<std::unique_ptr<Statement>>& statements)
{
    for(const auto& stmt : statements)
    {
        if(stmt->type == StatementType::ASSIGNMENT)
        {
            auto* assign = static_cast<Assignment*>(stmt.get());
            variables[assign->name] = assign->value;
        }
    }
}

void Interpreter::print_variables() const
{
    for(const auto& [name, value] : variables)
    {
        std::cout << name << " = " << value.to_string() << std::endl;
    }
}