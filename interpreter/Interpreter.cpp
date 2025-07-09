//
// Created by Denis on 08.07.2025.
//

#include "Interpreter.h"
#include "../parser/Evaluator.h"
#include <iostream>
#include <vector>

std::pair<std::string, Value> Interpreter::interpret(const std::vector<std::unique_ptr<Statement>>& statements)
{
    std::string last_name;
    Value last_value;

    for (const auto& stmt : statements)
    {
        if (stmt->type == StatementType::ASSIGNMENT)
        {
            auto* assign = dynamic_cast<Assignment*>(stmt.get());
            if(!assign)
            {
                std::cerr << "[ERROR] Invalid statement type in interpreter\n";
                continue;
            }

            Value result = evaluate(assign->value.get(), variables);

            variables[assign->name] = result;
            last_name = assign->name;
            last_value = result;
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