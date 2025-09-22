//
// Created by Denis on 09.07.2025.
//

#include "Evaluator.h"
#include "Expression.h"
#include "../builtin/Builtins.h"
#include <iostream>
#include <unordered_map>

static const std::unordered_map<std::string, FunctionStatement*>* g_user_functions = nullptr;

void set_user_functions(const std::unordered_map<std::string, FunctionStatement*>* ptr)
{
    g_user_functions = ptr;
}

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

        std::vector<Value> args;
        for(auto& arg : call->arguments)
        {
            args.push_back(evaluate(arg.get(), variables));
        }

        auto builtin_it = builtin_functions.find(function_name);
        if(builtin_it != builtin_functions.end()) {return builtin_it->second(args);}

        if(!g_user_functions) {std::cerr << "[ERROR] Unknown function: " << function_name << "\n"; return {};}

        auto user_it = g_user_functions->find(function_name);
        if(user_it == g_user_functions->end()) {std::cerr << "[ERROR] Unknown function: " << function_name << "\n"; return {};}

        FunctionStatement* fn = user_it->second;

        if(args.size() != fn->parameters.size()) {std::cerr << "[ERROR] Function " << function_name << " expects " << fn->parameters.size() << " args, got " << args.size() << "\n"; return {};}

        std::unordered_map<std::string, Value> local = variables;
        for(size_t i = 0; i < args.size(); ++i)
        {
            local[fn->parameters[i]] = args[i];
        }

        try
        {
            return evaluate(fn->body.get(), local);
        }
        catch(const ReturnException& e)
        {
            return e.value;
        }
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

    if(auto* assign_stmt = dynamic_cast<AssignmentStatement*>(stmt))
    {
        Value val = evaluate(assign_stmt->assignment.get(), variables);
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

    if(auto* while_stmt = dynamic_cast<WhileStatement*>(stmt))
    {
        while(true)
        {
            Value cond = evaluate(while_stmt->condition.get(), variables);

            bool truthy = false;
            if(cond.type == ValueType::INTEGER) truthy = std::get<int>(cond.data) != 0;
            else if(cond.type == ValueType::DOUBLE) truthy = std::get<double>(cond.data) != 0.0;
            else if(cond.type == ValueType::BOOLEAN) truthy = std::get<bool>(cond.data);
            else {std::cerr << "[ERROR] Invalid type in while-statement condition\n"; break;}

            if(!truthy) {break;}
            evaluate(while_stmt->body.get(), variables);
        }
        return {};
    }

    if(auto* repeat_stmt = dynamic_cast<RepeatStatement*>(stmt))
    {
        Value count_val = evaluate(repeat_stmt->count.get(), variables);
        int count = 0;

        if(count_val.type == ValueType::INTEGER) count = std::get<int>(count_val.data);
        else if(count_val.type == ValueType::DOUBLE) count = static_cast<int>(std::get<double>(count_val.data));
        else {std::cerr << "[ERROR] repeat count must be number\n"; return {};}

        for(int i = 0; i < count; ++i)
        {
            evaluate(repeat_stmt->body.get(), variables);
        }

        return {};
    }

    if(auto* for_stmt = dynamic_cast<ForStatement*>(stmt))
    {
        if(for_stmt->initializer) {evaluate(for_stmt->initializer.get(), variables);}

        while(true)
        {
            if(for_stmt->condition)
            {
                Value cond = evaluate(for_stmt->condition.get(), variables);
                bool truthy = false;

                if(cond.type == ValueType::BOOLEAN) truthy = std::get<bool>(cond.data);
                else if(cond.type == ValueType::INTEGER) truthy = std::get<int>(cond.data) != 0;
                else if(cond.type == ValueType::DOUBLE) truthy = std::get<double>(cond.data) != 0.0;
                else {std::cerr << "[ERROR] Invalid type in for-loop condition\n"; break;}

                if(!truthy) {break;}
            }

            evaluate(for_stmt->body.get(), variables);

            if(for_stmt->increment) {evaluate(for_stmt->increment.get(), variables);}
        }

        return {};
    }

    if(auto* ret = dynamic_cast<ReturnStatement*>(stmt))
    {
        Value v = evaluate(ret->value.get(), variables);
        throw ReturnException(v);
    }

    std::cerr << "[ERROR] Unknown statement type\n";
    return {};
}
