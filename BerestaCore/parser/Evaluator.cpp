//
// Created by Denis on 09.07.2025.
//

#include "Evaluator.h"
#include "Expression.h"
#include "../builtin/Builtins.h"
#include <iostream>
#include <unordered_map>

static std::unordered_map<std::string, std::unordered_map<std::string, int>> g_enums;
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

        if(unary->op == '!')
        {
            bool val = false;

            if(right_val.type == ValueType::BOOLEAN) {val = std::get<bool>(right_val.data);}
            else if(right_val.type == ValueType::INTEGER) {val = std::get<int>(right_val.data) != 0;}
            else if(right_val.type == ValueType::DOUBLE) {val = std::get<double>(right_val.data) != 0.0;}
            else if(right_val.type == ValueType::STRING) {val = !right_val.to_string().empty();}

            return Value(!val);
        }

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

    if(expr->type == ExpressionType::VARIABLE)
    {
        auto* var = dynamic_cast<VariableExpr*>(expr);
        return get_var(var->name);
    }

    if(expr->type == ExpressionType::STRING)
    {
        auto* s = dynamic_cast<StringExpr*>(expr);
        return Value(s->value);
    }

    if(expr->type == ExpressionType::BOOLEAN)
    {
        auto* b = dynamic_cast<BoolExpr*>(expr);
        return Value(b->value);
    }

    if(expr->type == ExpressionType::MEMBER_ACCESS)
    {
        auto* ma = dynamic_cast<MemberAccessExpr*>(expr);

        if(auto* ve = dynamic_cast<VariableExpr*>(ma->object.get()))
        {
            auto itE = g_enums.find(ve->name);
            if(itE != g_enums.end())
            {
                auto itM = itE->second.find(ma->member);
                if(itM != itE->second.end()) {return Value(itM->second);}

                std::cerr << "[ERROR] Unknown enum member: " << ve->name << "." << ma->member << "\n";
                return {};
            }
        }

        std::cerr << "[ERROR] Member access not supported for this expression type\n";
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

                    push_scope();
                    for(size_t i = 0; i < args.size(); ++i)
                    {
                        set_var(fn->parameters[i], args[i], true);
                    }

                    try
                    {
                        evaluate(fn->body.get(), env_stack.back());
                        pop_scope();
                        return {};
                    }
                    catch(const ReturnException& e)
                    {
                        pop_scope();
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

                std::string prev = g_current_file;
                g_current_file = def_file;

                push_scope();
                for(size_t i = 0; i < args.size(); ++i)
                {
                    set_var(fn->parameters[i], args[i], true);
                }

                try
                {
                    Value res = evaluate(fn->body.get(), env_stack.back());
                    pop_scope();
                    g_current_file = prev;
                    return res;
                }
                catch(const ReturnException& e)
                {
                    pop_scope();
                    g_current_file = prev;
                    return e.value;
                }
            }
        }

        std::cerr << "[ERROR] Unknown function: " << function_name << "\n";
        return {};
    }

    if(expr->type == ExpressionType::ARRAY_LITERAL)
    {
        auto* arr = dynamic_cast<ArrayLiteralExpr*>(expr);
        std::vector<Value> values;

        for(auto& e : arr->elements)
        {
            values.push_back(evaluate(e.get(), variables));
        }

        return Value(values);
    }

    if(expr->type == ExpressionType::INDEX)
    {
        auto* idx = dynamic_cast<IndexExpr*>(expr);
        Value container_val = evaluate(idx->array.get(), variables);
        Value index_val = evaluate(idx->index.get(), variables);

        if(container_val.type == ValueType::ARRAY)
        {
            int i = 0;
            if(index_val.type == ValueType::INTEGER) {i = std::get<int>(index_val.data);}
            else if(index_val.type == ValueType::DOUBLE) {i = static_cast<int>(std::get<double>(index_val.data));}
            else {std::cerr << "[ERROR] Array index must be a number (int or double)\n"; return {};}

            auto& arr = std::get<std::vector<Value>>(container_val.data);
            if(i < 0 || i >= static_cast<int>(arr.size())) {std::cerr << "[ERROR] Array index out of bounds\n"; return {};}

            return arr[i];
        }

        if(container_val.type == ValueType::DICTIONARY)
        {
            std::string key;

            if(index_val.type == ValueType::STRING) {key = std::get<std::string>(index_val.data);}
            else {key = index_val.to_string();}

            const auto& dict = *std::get<DictionaryPtr>(container_val.data);
            auto it = dict.find(key);

            if(it == dict.end()) {std::cerr << "[ERROR] Key not found in directory: " << key << "\n"; return {};}

            return it->second;
        }

        std::cerr << "[ERROR] Indexing non-array/non-dictionary value\n";
        return {};
    }

    if(expr->type == ExpressionType::DICTIONARY_LITERAL)
    {
        auto* dict_expr = dynamic_cast<DictionaryLiteralExpr*>(expr);
        Dictionary dict;

        for(auto& [key_expr, val_expr] : dict_expr->entries)
        {
            Value k = evaluate(key_expr.get(), variables);
            Value v = evaluate(val_expr.get(), variables);

            if(k.type != ValueType::STRING) {std::cerr << "[ERROR] Dictionary keys must be strings\n"; return {};}

            dict[std::get<std::string>(k.data)] = v;
        }

        return Value(dict);
    }

    std::cerr << "[ERROR] Unknown expression type" << std::endl;
    return {};
}

Value evaluate(Statement* stmt, std::unordered_map<std::string, Value>& variables)
{
    if(auto* assign = dynamic_cast<Assignment*>(stmt))
    {
        Value val = evaluate(assign->value.get(), env_stack.back());
        set_var(assign->name, val, assign->is_let);
        return val;
    }

    if(auto* assign_stmt = dynamic_cast<AssignmentStatement*>(stmt))
    {
        Value val = evaluate(assign_stmt->assignment.get(), variables);
        return val;
    }

    if(auto* expr_stmt = dynamic_cast<ExpressionStatement*>(stmt)) {return evaluate(expr_stmt->expression.get(), variables);}

    if(auto* block_stmt = dynamic_cast<BlockStatement*>(stmt))
    {
        Value result;
        for(const auto& s : block_stmt->statements)
        {
            result = evaluate(s.get(), variables);
        }
        return result;
    }

    if(auto* enum_stmt = dynamic_cast<EnumStatement*>(stmt))
    {
        g_enums[enum_stmt->name] = enum_stmt->members;
        return {};
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

    if(auto* foreach_stmt = dynamic_cast<ForeachStatement*>(stmt))
    {
        Value iterable_val = evaluate(foreach_stmt->iterable.get(), variables);

        if(iterable_val.type != ValueType::ARRAY) {std::cerr << "[ERROR] Foreach expects an array\n"; return {};}

        auto& arr = std::get<std::vector<Value>>(iterable_val.data);
        for(auto& elem : arr)
        {
            push_scope();
            set_var(foreach_stmt->var_name, elem, true);
            evaluate(foreach_stmt->body.get(), env_stack.back());
            pop_scope();
        }

        return {};
    }

    if(auto* return_stmt = dynamic_cast<ReturnStatement*>(stmt))
    {
        Value v = return_stmt->value ? evaluate(return_stmt->value.get(), variables) : Value();
        throw ReturnException(v);
    }

    if(auto* ia_stmt = dynamic_cast<IndexAssignment*>(stmt))
    {
        std::vector<Value> raw_indices;
        Expression* cur = ia_stmt->target.get();

        while(auto* idx = dynamic_cast<IndexExpr*>(cur))
        {
            Value iv = evaluate(idx->index.get(), variables);
            raw_indices.push_back(iv);
            cur = idx->array.get();
        }

        auto* var = dynamic_cast<VariableExpr*>(cur);
        if(!var) {std::cerr << "[ERROR] Left side of indexed assignment must be a variable\n"; return {};}

        Value container_val = get_var(var->name);
        Value new_val = evaluate(ia_stmt->value.get(), variables);

        std::reverse(raw_indices.begin(), raw_indices.end());
        Value* current = &container_val;

        for(size_t level = 0; level < raw_indices.size(); ++level)
        {
            Value idx = raw_indices[level];
            bool last = (level + 1 == raw_indices.size());

            if(current->type == ValueType::ARRAY)
            {
                int i = 0;
                if(idx.type == ValueType::INTEGER) {i = std::get<int>(idx.data);}
                else if(idx.type == ValueType::DOUBLE) {i = static_cast<int>(std::get<double>(idx.data));}
                else {std::cerr << "[ERROR] Array index must be a number (int or double)\n"; return {};}

                auto& vec = std::get<std::vector<Value>>(current->data);

                if(i < 0) {std::cerr << "[ERROR] Negative array index\n"; return {};}
                if(static_cast<size_t>(i) >= vec.size()) vec.resize(i + 1, Value());

                if(last) {vec[i] = new_val;}
                else {current = &vec[i];}
            }
            else if(current->type == ValueType::DICTIONARY)
            {
                std::string key = (idx.type == ValueType::STRING) ? std::get<std::string>(idx.data) : idx.to_string();

                auto& dict = *std::get<DictionaryPtr>(current->data);

                if(last) {dict[key] = new_val;}
                else
                {
                    auto it = dict.find(key);
                    if(it == dict.end())
                    {
                        dict[key] = Value(Dictionary{});
                        current = &dict[key];
                    }
                    else {current = &it->second;}
                }
            }
            else {std::cerr << "[ERROR] Indexed assignment not supported for this type\n"; return {};}
        }

        set_var(var->name, container_val, false);
        return new_val;
    }

    std::cerr << "[ERROR] Unknown statement type\n";
    return {};
}
