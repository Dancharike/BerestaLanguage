//
// Created by Denis on 09.07.2025.
//

#include "Evaluator.h"
#include <iostream>
#include <unordered_map>

Value evaluate(Expression* expr, const std::unordered_map<std::string, Value>& variables)
{
    if(expr->type == ExpressionType::UNARY)
    {
        auto* unary = dynamic_cast<UnaryExpr*>(expr);
        Value right_val = evaluate(unary->right.get(), variables);

        if(right_val.type == ValueType::DOUBLE)
        {
            double val = std::get<double>(right_val.data);
            switch(unary->op)
            {
                case '-': return Value(-val);
                case '+': return Value(+val);
                default: std::cerr << "[ERROR] Unknown unary operator: " << unary->op << std::endl; return {};
            }
        }

        if(right_val.type == ValueType::INTEGER)
        {
            int val = std::get<int>(right_val.data);
            switch(unary->op)
            {
                case '-': return Value(-val);
                case '+': return Value(+val);
                default: std::cerr << "[ERROR] Unknown unary operator: " << unary->op << std::endl; return {};
            }
        }

        std::cerr << "[ERROR] Unsupported unary operand type" << std::endl;
        return {};
    }

    if(expr->type == ExpressionType::NUMBER)
    {
        return dynamic_cast<NumberExpr*>(expr)->value;
    }

    if(expr->type == ExpressionType::VARIABLE)
    {
        auto* var = dynamic_cast<VariableExpr*>(expr);
        auto it = variables.find(var->name);
        if (it != variables.end()) { return it->second; }

        std::cerr << "[ERROR] Variable not found: " << var->name << std::endl;
        return {};
    }
    /*
    if(expr->type == ExpressionType::BINARY)
    {
        auto* bin = dynamic_cast<BinaryExpr*>(expr);
        Value left_val = evaluate(bin->left.get(), variables);
        Value right_val = evaluate(bin->right.get(), variables);

        if((left_val.type == ValueType::INTEGER|| left_val.type == ValueType::DOUBLE) && (right_val.type == ValueType::INTEGER || right_val.type == ValueType::DOUBLE))
        {
            double l = (left_val.type == ValueType::DOUBLE) ? std::get<double>(left_val.data) : std::get<int>(left_val.data);
            double r = (right_val.type == ValueType::DOUBLE) ? std::get<double>(right_val.data) : std::get<int>(right_val.data);

            switch(bin->op)
            {
                case '+': return Value(l + r);
                case '-': return Value(l - r);
                case '*': return Value(l * r);
                case '/': return (r != 0.0) ? Value(l / r) : Value(0.0);
                default: std::cerr << "[ERROR] Unknown binary operator: " << bin->op << std::endl; return {};
            }
        }

        if(left_val.type == ValueType::INTEGER && right_val.type == ValueType::INTEGER)
        {
            int l = std::get<int>(left_val.data);
            int r = std::get<int>(right_val.data);

            switch (bin->op)
            {
                case '+': return Value(l + r);
                case '-': return Value(l - r);
                case '*': return Value(l * r);
                case '/': return (r != 0) ? Value(l / r) : Value(0);
                default: std::cerr << "[ERROR] Unknown binary operator: " << bin->op << std::endl; return {};
            }
        }

        std::cerr << "[ERROR] Unsupported operand types" << std::endl;
        return {};
    }
    */

    if(expr->type == ExpressionType::BINARY)
    {
        auto* bin = dynamic_cast<BinaryExpr*>(expr);
        Value left_val = evaluate(bin->left.get(), variables);
        Value right_val = evaluate(bin->right.get(), variables);
        const std::string& op = bin->op;

        if((left_val.type == ValueType::INTEGER || left_val.type == ValueType::DOUBLE) && (right_val.type == ValueType::INTEGER || right_val.type == ValueType::DOUBLE))
        {
            double l = (left_val.type == ValueType::DOUBLE) ? std::get<double>(left_val.data) : std::get<int>(left_val.data);
            double r = (right_val.type == ValueType::DOUBLE) ? std::get<double>(right_val.data) : std::get<int>(right_val.data);

            if(op == "+") return Value(l + r);
            if(op == "-") return Value(l - r);
            if(op == "*") return Value(l * r);
            if(op == "/") return (r != 0.0) ? Value(l / r) : Value(0.0);

            if(op == "==") return Value(l == r);
            if(op == "!=") return Value(l != r);
            if(op == "<")  return Value(l <  r);
            if(op == "<=") return Value(l <= r);
            if(op == ">")  return Value(l >  r);
            if(op == ">=") return Value(l >= r);
        }

        if(left_val.type == ValueType::BOOLEAN && right_val.type == ValueType::BOOLEAN)
        {
            bool l = std::get<bool>(left_val.data);
            bool r = std::get<bool>(right_val.data);

            if(op == "==") return Value(l == r);
            if(op == "!=") return Value(l != r);
            if(op == "and" || op == "&&") return Value(l && r);
            if(op == "or" || op == "||") return Value(l || r);
        }

        std::cerr << "[ERROR] Unsupported operand types" << std::endl;
        return {};
    }

    if(expr->type == ExpressionType::IF)
    {
        auto* if_expr = dynamic_cast<IfExpr*>(expr);
        Value condition_val = evaluate(if_expr->condition.get(), variables);

        bool condition_truthy = false;
        if(condition_val.type == ValueType::INTEGER) {condition_truthy = std::get<int>(condition_val.data) != 0;}
        else if(condition_val.type == ValueType::DOUBLE) {condition_truthy = std::get<double>(condition_val.data) != 0.0;}
        else if(condition_val.type == ValueType::BOOLEAN) {condition_truthy = std::get<bool>(condition_val.data);}
        else {std::cerr << "[ERROR] Invalid condition type in if-expression\n"; return {};}

        if(condition_truthy) {return evaluate(if_expr->then_branch.get(), variables);}
        else if(if_expr->else_branch) {return evaluate(if_expr->else_branch.get(), variables);}
        else {return {};}
    }

    if(expr->type == ExpressionType::BLOCK)
    {
        auto* block = dynamic_cast<BlockExpr*>(expr);
        Value result;

        for(const auto& stmt : block->statements)
        {
            result = evaluate(stmt.get(), variables);
        }

        return result;
    }

    if(expr->type == ExpressionType::CONSOLE_PRINT)
    {
        auto* print_expr = dynamic_cast<ConsolePrintExpr*>(expr);
        Value value = evaluate(print_expr->expression.get(), variables);
        std::cout << value.to_string() << std::endl;
        return value;
    }

    std::cerr << "[ERROR] Unknown expression type" << std::endl;
    return {};
}
