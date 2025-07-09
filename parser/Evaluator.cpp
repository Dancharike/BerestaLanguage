//
// Created by Denis on 09.07.2025.
//

#include "Evaluator.h"
#include <iostream>

Value evaluate(Expression* expr)
{
    if(expr->type == ExpressionType::NUMBER)
    {
        return static_cast<NumberExpr*>(expr)->value;
    }

    if(expr->type == ExpressionType::BINARY)
    {
        auto* bin = static_cast<BinaryExpr*>(expr);
        Value left = evaluate(bin->left.get());
        Value right = evaluate(bin->right.get());

        if(left.type != ValueType::INTEGER || right.type != ValueType::INTEGER)
        {
            std::cout << "Error: expression doesn't contain int!\n";
            std::cout << "Left Type: " << static_cast<int>(left.type) << ", Right Type: " << static_cast<int>(right.type) << "\n";
            return Value();
        }

        int l = std::get<int>(left.data);
        int r = std::get<int>(right.data);

        switch (bin->op)
        {
            case '+': return Value(l + r);
            case '-': return Value(l - r);
            case '*': return Value(l * r);
            case '/': return Value(r != 0 ? l / r : 0);
            default: return Value();
        }
    }

    return Value();
}
