//
// Created by Denis on 09.07.2025.
//

#include "Evaluator.h"
#include "Expression.h"
#include "../builtin/Builtins.h"
#include <iostream>
#include <unordered_map>

static const PublicFunctionMap* g_public = nullptr;
static const PrivateFunctionMap* g_private = nullptr;
static std::string g_current_file;

void set_public_functions(const PublicFunctionMap* ptr) {g_public = ptr;}
void set_private_functions(const PrivateFunctionMap* ptr) {g_private = ptr;}
void set_current_file(const std::string& filename) {g_current_file = filename;}

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

    if(expr->type == ExpressionType::NUMBER) {return dynamic_cast<NumberExpr*>(expr)->value;}

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

        if(left_val.type == ValueType::STRING || right_val.type == ValueType::STRING)
        {
            if(op == "+") {return Value(left_val.to_string() + right_val.to_string());}
        }

        std::cerr << "[ERROR] Unsupported operand types" << std::endl;
        return {};
    }

    if(expr->type == ExpressionType::STRING)
    {
        auto* s = dynamic_cast<StringExpr*>(expr);
        return Value(s->value);
    }

    if(expr->type == ExpressionType::FUNCTION_CALL)
    {
        auto* call = dynamic_cast<FunctionCallExpr*>(expr);
        auto* var_expr = dynamic_cast<VariableExpr*>(call->callee.get());
        if(!var_expr) {std::cerr << "[ERROR] Only variable-style calls supported (e.g. foo(1))\n"; return {};}
        std::string function_name = var_expr->name;

        std::vector<Value> args;
        for(auto& arg : call->arguments) { args.push_back(evaluate(arg.get(), variables)); }

        auto itb = builtin_functions.find(function_name);
        if(itb != builtin_functions.end()) { return itb->second(args); }

        if(g_private)
        {
            auto it_file = g_private->find(g_current_file);
            if(it_file != g_private->end())
            {
                auto itp = it_file->second.find(function_name);
                if(itp != it_file->second.end())
                {
                    FunctionStatement* fn = itp->second;
                    if(args.size() != fn->parameters.size()) {std::cerr << "[ERROR] Function " << function_name << " expects " << fn->parameters.size() << " args, got " << args.size() << "\n"; return {};}
                    std::unordered_map<std::string, Value> local = variables;

                    for(size_t i = 0; i < args.size(); ++i)
                    {
                        local[fn->parameters[i]] = args[i];
                    }

                    try
                    {
                        evaluate(fn->body.get(), local);
                        return {};
                    }
                    catch(const ReturnException& e)
                    {
                        return e.value;
                    }
                }
            }
        }

        if(g_public)
        {
            auto itg = g_public->find(function_name);
            if(itg != g_public->end())
            {
                FunctionStatement* fn = itg->second.first;
                const std::string& def_file = itg->second.second;

                if(args.size() != fn->parameters.size()) {std::cerr << "[ERROR] Function " << function_name << " expects " << fn->parameters.size() << " args, got " << args.size() << "\n"; return {};}

                std::unordered_map<std::string, Value> local = variables;
                for(size_t i = 0; i < args.size(); ++i)
                {
                    local[fn->parameters[i]] = args[i];
                }

                std::string prev = g_current_file;
                g_current_file = def_file;
                try
                {
                    Value res = evaluate(fn->body.get(), local);
                    g_current_file = prev;
                    return res;
                }
                catch(const ReturnException& e)
                {
                    g_current_file = prev;
                    return e.value;
                }
            }
        }

        std::cerr << "[ERROR] Unknown function: " << function_name << "\n";
        return {};
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
        Value v = ret->value ? evaluate(ret->value.get(), variables) : Value();
        throw ReturnException(v);
    }

    std::cerr << "[ERROR] Unknown statement type\n";
    return {};
}
