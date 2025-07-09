//
// Created by Denis on 09.07.2025.
//

#include "Evaluator.h"
#include <iostream>
#include <unordered_map>

Value evaluate(Expression* expr, const std::unordered_map<std::string, Value>& variables)
{
    if (expr->type == ExpressionType::NUMBER)
    {
        return dynamic_cast<NumberExpr*>(expr)->value;
    }

    if (expr->type == ExpressionType::VARIABLE)
    {
        auto* var = dynamic_cast<VariableExpr*>(expr);
        auto it = variables.find(var->name);
        if (it != variables.end()) { return it->second; }

        std::cerr << "[ERROR] Variable not found: " << var->name << std::endl;
        return {};
    }

    if (expr->type == ExpressionType::BINARY)
    {
        auto* bin = dynamic_cast<BinaryExpr*>(expr);
        Value left_val = evaluate(bin->left.get(), variables);
        Value right_val = evaluate(bin->right.get(), variables);

        if (left_val.type == ValueType::INTEGER && right_val.type == ValueType::INTEGER)
        {
            int l = std::get<int>(left_val.data);
            int r = std::get<int>(right_val.data);

            switch (bin->op)
            {
                case '+': return Value(l + r);
                case '-': return Value(l - r);
                case '*': return Value(l * r);
                case '/': return (r != 0) ? Value(l / r) : Value(0);
                default:
                    std::cerr << "[ERROR] Unknown binary operator: " << bin->op << std::endl;
                    return {};
            }
        }

        std::cerr << "[ERROR] Unsupported operand types" << std::endl;
        return {};
    }

    std::cerr << "[ERROR] Unknown expression type" << std::endl;
    return {};
}
