//
// Created by Denis on 09.07.2025.
//

#include "Evaluator.h"
#include "Expression.h"
#include "../builtin/Builtins.h"
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

    if(expr->type == ExpressionType::FUNCTION_CALL)
    {
        auto* call = dynamic_cast<FunctionCallExpr*>(expr);
        auto* var_expr = dynamic_cast<VariableExpr*>(call->callee.get());

        if(!var_expr) {std::cerr << "[ERROR] Only variable-style calls supported (e.g. foo(1))\n"; return {};}

        std::string function_name = var_expr->name;

        if(builtin_functions.find(function_name) == builtin_functions.end()) {std::cerr << "[ERROR] Unknown function: " << function_name << "\n"; return {};}

        std::vector<Value> args;
        for (auto& arg : call->arguments)
        {
            args.push_back(evaluate(arg.get(), variables));
        }

        return builtin_functions.at(function_name)(args);
    }

    std::cerr << "[ERROR] Unknown expression type" << std::endl;
    return {};
}

Value evaluate(Statement* stmt, std::unordered_map<std::string, Value>& variables)
{
    if(auto* assign = dynamic_cast<Assignment*>(stmt))
    {
        Value val = evaluate(assign->value.get(), variables);
        variables[assign->name] = val;
        return val;
    }

    if(auto* expr_stmt = dynamic_cast<ExpressionStatement*>(stmt)) {return evaluate(expr_stmt->expression.get(), variables);}

    if(auto* block = dynamic_cast<BlockStatement*>(stmt))
    {
        Value result;
        for(const auto& s : block->statements)
        {
            result = evaluate(s.get(), variables);
        }
        return result;
    }

    if(auto* if_stmt = dynamic_cast<IfStatement*>(stmt))
    {
        Value cond = evaluate(if_stmt->condition.get(), variables);

        bool truthy = false;
        if(cond.type == ValueType::INTEGER) truthy = std::get<int>(cond.data) != 0;
        else if(cond.type == ValueType::DOUBLE) truthy = std::get<double>(cond.data) != 0.0;
        else if(cond.type == ValueType::BOOLEAN) truthy = std::get<bool>(cond.data);
        else {std::cerr << "[ERROR] Invalid type in if-statement condition\n"; return {};}

        return truthy ? evaluate(if_stmt->then_branch.get(), variables)
            : (if_stmt->else_branch ? evaluate(if_stmt->else_branch.get(), variables) : Value());
    }

    std::cerr << "[ERROR] Unknown statement type\n";
    return {};
}
