//
// Created by Denis on 09.07.2025.
//

#include "Evaluator.h"
#include "Expression.h"
#include "Statement.h"
#include "../interpreter/FunctionIndex.h"
#include "../builtin/Builtins.h"
#include <iostream>
#include <unordered_map>

Evaluator::Evaluator(Environment &env, FunctionIndex &index, std::string current_file) : _env(env), _index(index)
{
    _file_stack.push_back(std::move(current_file));
}

Value Evaluator::eval_expression(Expression* expr)
{
    if(!expr) {return {};}

    switch(expr->type)
    {
        case ExpressionType::NUMBER: {return dynamic_cast<NumberExpr *>(expr)->value;}

        case ExpressionType::STRING: {return Value(dynamic_cast<StringExpr *>(expr)->value);}

        case ExpressionType::BOOLEAN: {return Value(dynamic_cast<BoolExpr *>(expr)->value);}

        case ExpressionType::VARIABLE:
        {
            auto* var = dynamic_cast<VariableExpr*>(expr);
            return _env.get(var->name);
        }

        case ExpressionType::UNARY:
        {
            auto* unary = dynamic_cast<UnaryExpr*>(expr);
            Value right_val = eval_expression(unary->right.get());

            switch(unary->op)
            {
                case '+':
                {
                    if(right_val.type == ValueType::INTEGER) {return Value(std::get<int>(right_val.data));}
                    if(right_val.type == ValueType::DOUBLE)  {return Value(std::get<double>(right_val.data));}
                    break;
                }

                case '-':
                {
                    if(right_val.type == ValueType::INTEGER) {return Value(-std::get<int>(right_val.data));}
                    if(right_val.type == ValueType::DOUBLE)  {return Value(-std::get<double>(right_val.data));}
                    break;
                }

                case '!':
                {
                    bool val = false;
                    if(right_val.type == ValueType::BOOLEAN)      {val = std::get<bool>(right_val.data);}
                    else if(right_val.type == ValueType::INTEGER) {val = std::get<int>(right_val.data) != 0;}
                    else if(right_val.type == ValueType::DOUBLE)  {val = std::get<double>(right_val.data) != 0.0;}
                    else if(right_val.type == ValueType::STRING)  {val = !right_val.to_string().empty();}
                    return Value(!val);
                }
            }

            std::cerr << "[ERROR] Unsupported unary operand type operator\n";
            return {};
        }

        case ExpressionType::BINARY:
        {
            auto* bin = dynamic_cast<BinaryExpr*>(expr);
            Value left = eval_expression(bin->left.get());
            Value right = eval_expression(bin->right.get());
            const std::string& op = bin->op;

            if((left.type == ValueType::INTEGER || left.type == ValueType::DOUBLE) && (right.type == ValueType::INTEGER || right.type == ValueType::DOUBLE))
            {
                double l = (left.type == ValueType::DOUBLE) ? std::get<double>(left.data) : std::get<int>(left.data);
                double r = (right.type == ValueType::DOUBLE) ? std::get<double>(right.data) : std::get<int>(right.data);

                if(op == "+") {return Value(l + r);}
                if(op == "-") {return Value(l - r);}
                if(op == "*") {return Value(l * r);}
                if(op == "/") {return (r != 0.0) ? Value(l / r) : Value(0.0);}

                if(op == "==") {return Value(l == r);}
                if(op == "!=") {return Value(l != r);}
                if(op == "<")  {return Value(l <  r);}
                if(op == "<=") {return Value(l <= r);}
                if(op == ">")  {return Value(l >  r);}
                if(op == ">=") {return Value(l >= r);}
            }

            if(left.type == ValueType::BOOLEAN && right.type == ValueType::BOOLEAN)
            {
                bool l = std::get<bool>(left.data);
                bool r = std::get<bool>(right.data);

                if(op == "==") {return Value(l == r);}
                if(op == "!=") {return Value(l != r);}
                if(op == "and" || op == "&&") {return Value(l && r);}
                if(op == "or"  || op == "||") {return Value(l || r);}
            }

            if(op == "+" && (left.type == ValueType::STRING || right.type == ValueType::STRING))
            {
                return Value(left.to_string() + right.to_string());
            }

            std::cerr << "[ERROR] Unsupported operand types for binary operator '" << op << "'\n";
            return {};
        }

        case ExpressionType::FUNCTION_CALL:
        {
            auto* call = dynamic_cast<FunctionCallExpr*>(expr);
            auto* callee_var = dynamic_cast<VariableExpr*>(call->callee.get());

            if(!callee_var) {std::cerr << "[ERROR] Only simple function names supported (e.g. foo(1))\n"; return {};}

            std::string fn_name = callee_var->name;

            std::vector<Value> args;
            for(auto& arg : call->arguments)
            {
                args.push_back(eval_expression(arg.get()));
            }

            if(auto it = builtin_functions.find(fn_name); it != builtin_functions.end())
            {
                try {return it->second(args);}
                catch(...) {std::cerr << "[ERROR] Exception in builtin function: " << fn_name << "\n"; return {};}
            }

            const FunctionRef* ref = _index.find_function(fn_name, current_file());
            if(!ref) {std::cerr << "[ERROR] Unknown function: " << fn_name << "\n"; return {};}

            FunctionStatement* fn = ref->func;
            if(args.size() != fn->parameters.size()) {std::cerr << "[ERROR] Function " << fn_name << " expects " << fn->parameters.size() << " args, got " << args.size() << "\n"; return {};}

            bool pushed_file = false;
            if(ref->is_public && ref->file != current_file())
            {
                _file_stack.push_back(ref->file);
                pushed_file = true;
            }

            _env.push_scope();
            for(size_t i = 0; i < args.size(); ++i)
            {
                _env.define(fn->parameters[i], args[i]);
            }

            Value result;
            try
            {
                result = eval_statement(fn->body.get());
            }
            catch(const ReturnException& e)
            {
                result = e.value;
            }

            _env.pop_scope();
            if(pushed_file) {_file_stack.pop_back();}
            return result;
        }

        case ExpressionType::ARRAY_LITERAL:
        {
            auto* arr = dynamic_cast<ArrayLiteralExpr*>(expr);
            std::vector<Value> elems;
            for(auto& e : arr->elements)
            {
                elems.push_back(eval_expression(e.get()));
            }

            return Value(elems);
        }

        case ExpressionType::DICTIONARY_LITERAL:
        {
            auto* dict_expr = dynamic_cast<DictionaryLiteralExpr*>(expr);
            Dictionary dict;
            for(auto& [key, val] : dict_expr->entries)
            {
                Value k = eval_expression(key.get());
                Value v = eval_expression(val.get());
                if(k.type != ValueType::STRING) {std::cerr << "[ERROR] Dictionary key must be string literal\n"; return {};}
                dict[std::get<std::string>(k.data)] = v;
            }
            return Value(dict);
        }

        case ExpressionType::INDEX:
        {
            auto* idx = dynamic_cast<IndexExpr*>(expr);
            Value container = eval_expression(idx->array.get());
            Value index_val = eval_expression(idx->index.get());

            if(container.type == ValueType::ARRAY)
            {
                int i = 0;
                if(index_val.type == ValueType::INTEGER)     {i = std::get<int>(index_val.data);}
                else if(index_val.type == ValueType::DOUBLE) {i = static_cast<int>(std::get<double>(index_val.data));}
                else                                         {std::cerr << "[ERROR] Array index must be numeric\n"; return {};}

                auto& arr = std::get<std::vector<Value>>(container.data);
                if(i < 0 || i >= static_cast<int>(arr.size())) {std::cerr << "[ERROR] Array index out of bounds\n"; return {};}

                return arr[i];
            }

            if(container.type == ValueType::DICTIONARY)
            {
                std::string key;
                if(index_val.type == ValueType::STRING)  {key = std::get<std::string>(index_val.data);}
                else                                     {key = index_val.to_string();}

                const auto &dict = *std::get<DictionaryPtr>(container.data);
                auto it = dict.find(key);
                if(it == dict.end()) {std::cerr << "[ERROR] Key not found in dictionary: " << key << "\n"; return {};}
                return it->second;
            }
        }

        default: std::cerr << "[WARN] Expression type not yet implemented!\n"; return {};
    }
}

Value Evaluator::eval_statement(Statement* stmt)
{
    if(!stmt) return {};

    switch(stmt->type)
    {
        case StatementType::ASSIGNMENT:
        {
            auto* assign = dynamic_cast<Assignment*>(stmt);
            Value val = eval_expression(assign->value.get());
            if(assign->is_let) {_env.define(assign->name, val);}
            else               {_env.assign(assign->name, val);}
            return val;
        }

        case StatementType::ASSIGNMENT_STATEMENT:
        {
            auto* assign_stmt = dynamic_cast<AssignmentStatement*>(stmt);
            return eval_statement(assign_stmt->assignment.get());
        }

        case StatementType::EXPRESSION:
        {
            auto* expr_stmt = dynamic_cast<ExpressionStatement*>(stmt);
            return eval_expression(expr_stmt->expression.get());
        }

        case StatementType::IF:
        {
            auto* if_stmt = dynamic_cast<IfStatement*>(stmt);
            Value cond = eval_expression(if_stmt->condition.get());

            bool truthy = false;
            if(cond.type == ValueType::BOOLEAN)      {truthy = std::get<bool>(cond.data);}
            else if(cond.type == ValueType::INTEGER) {truthy = std::get<int>(cond.data) != 0;}
            else if(cond.type == ValueType::DOUBLE)  {truthy = std::get<double>(cond.data) != 0.0;}
            else                                     {std::cerr << "[ERROR] Invalid condition type in if-statement\n";}

            if(truthy) {return eval_statement(if_stmt->then_branch.get());}
            else if(if_stmt->else_branch) {return eval_statement(if_stmt->else_branch.get());}

            return {};
        }

        case StatementType::WHILE:
        {
            auto* while_stmt = dynamic_cast<WhileStatement*>(stmt);
            Value result;

            while(true)
            {
                Value cond = eval_expression(while_stmt->condition.get());
                bool truthy = false;

                if(cond.type == ValueType::BOOLEAN)      {truthy = std::get<bool>(cond.data);}
                else if(cond.type == ValueType::INTEGER) {truthy = std::get<int>(cond.data) != 0;}
                else if(cond.type == ValueType::DOUBLE)  {truthy = std::get<double>(cond.data) != 0.0;}
                else                                     {std::cerr << "[ERROR] Invalid while condition type\n";}

                if(!truthy) {break;}

                result = eval_statement(while_stmt->body.get());
            }

            return result;
        }

        case StatementType::REPEAT:
        {
            auto* repeat_stmt = dynamic_cast<RepeatStatement*>(stmt);
            Value count_val = eval_expression(repeat_stmt->count.get());

            int count = 0;
            if(count_val.type == ValueType::INTEGER)     {count = std::get<int>(count_val.data);}
            else if(count_val.type == ValueType::DOUBLE) {count = static_cast<int>(std::get<double>(count_val.data));}
            else                                         {std::cerr << "[ERROR] repeat() count must be numeric\n"; return {};}

            Value result;
            for(int i = 0; i < count; ++i)
            {
                result = eval_statement(repeat_stmt->body.get());
            }

            return result;
        }

        case StatementType::FOR:
        {
            auto* for_stmt = dynamic_cast<ForStatement*>(stmt);
            Value result;

            _env.push_scope();
            if(for_stmt->initializer) {eval_statement(for_stmt->initializer.get());}

            while(true)
            {
                bool truthy = true;
                if(for_stmt->condition)
                {
                    Value cond = eval_expression(for_stmt->condition.get());

                    if(cond.type == ValueType::BOOLEAN)      {truthy = std::get<bool>(cond.data);}
                    else if(cond.type == ValueType::INTEGER) {truthy = std::get<int>(cond.data) != 0;}
                    else if(cond.type == ValueType::DOUBLE)  {truthy = std::get<double>(cond.data) != 0.0;}
                    else                                     {std::cerr << "[ERROR] Invalid for-loop condition type\n"; break;}
                }

                if(!truthy) {break;}

                result = eval_statement(for_stmt->body.get());

                if(for_stmt->increment)
                {
                    eval_statement(for_stmt->increment.get());
                }
            }

            _env.pop_scope();
            return result;
        }

        case StatementType::FOREACH:
        {
            auto* foreach_stmt = dynamic_cast<ForeachStatement*>(stmt);
            Value iterable = eval_expression(foreach_stmt->iterable.get());

            if(iterable.type != ValueType::ARRAY) {std::cerr << "[ERROR] foreach expects an array\n"; return {};}

            const auto& arr = std::get<std::vector<Value>>(iterable.data);
            Value result;

            for(const auto& elem : arr)
            {
                _env.push_scope();
                _env.define(foreach_stmt->var_name, elem);
                result = eval_statement(foreach_stmt->body.get());
                _env.pop_scope();
            }

            return result;
        }

        // тут ничего не надо, просто указать такой тип, ибо функции уже до этого момента проиндексированы
        case StatementType::FUNCTION: {return {};}

        case StatementType::BLOCK:
        {
            auto* block = dynamic_cast<BlockStatement*>(stmt);
            _env.push_scope();
            Value result;
            for(const auto& s : block->statements)
            {
                result = eval_statement(s.get());
            }
            _env.pop_scope();
            return result;
        }

        case StatementType::RETURN:
        {
            auto* ret = dynamic_cast<ReturnStatement*>(stmt);
            Value val = ret->value ? eval_expression(ret->value.get()) : Value();
            throw ReturnException(val);
        }

        case StatementType::INDEX_ASSIGNMENT:
        {
            auto* ia = dynamic_cast<IndexAssignment*>(stmt);

            std::vector<Value> indices;
            Expression* target_expr = ia->target.get();
            while(auto* idx = dynamic_cast<IndexExpr*>(target_expr))
            {
                indices.push_back(eval_expression(idx->index.get()));
                target_expr = idx->array.get();
            }

            auto* var = dynamic_cast<VariableExpr*>(target_expr);
            if(!var) {std::cerr << "[ERROR] Indexed assignment target must be variable\n"; return {};}

            Value container = _env.get(var->name);
            Value new_val = eval_expression(ia->value.get());
            std::reverse(indices.begin(), indices.end());
            Value* cur = &container;

            for(size_t level = 0; level < indices.size(); ++level)
            {
                Value idx_val = indices[level];
                bool last = (level + 1 == indices.size());

                if(cur->type == ValueType::ARRAY)
                {
                    int i = (idx_val.type == ValueType::INTEGER) ? std::get<int>(idx_val.data) : static_cast<int>(std::get<double>(idx_val.data));
                    auto& arr = std::get<std::vector<Value>>(cur->data);
                    if(i < 0)                             {std::cerr << "[ERROR] Negative array index\n"; return {};}
                    if(i >= static_cast<int>(arr.size())) {arr.resize(i + 1, Value());}
                    if(last)                              {arr[i] = new_val;}
                    else                                  {cur = &arr[i];}
                }
                else if(cur->type == ValueType::DICTIONARY)
                {
                    std::string key = (idx_val.type == ValueType::STRING) ? std::get<std::string>(idx_val.data) : idx_val.to_string();
                    auto& dict = *std::get<DictionaryPtr>(cur->data);
                    if(last) {dict[key] = new_val;}
                    else
                    {
                        auto it = dict.find(key);
                        if(it == dict.end())
                        {
                            dict[key] = Value(Dictionary{});
                            cur = &dict[key];
                        }
                        else {cur = &it->second;}
                    }
                }
                else {std::cerr << "[ERROR] Indexed assignment not supported for this type\n"; return {};}
            }

            _env.assign(var->name, container);
            return new_val;
        }

        case StatementType::ENUM:
        {
            auto* enum_stmt = dynamic_cast<EnumStatement*>(stmt);
            for(const auto& [name, val] : enum_stmt->members)
            {
                _env.define(enum_stmt->name + "." + name, Value(val));
            }
            return {};
        }

        default: std::cerr << "[WARN] Statement type not yet implemented\n"; return {};
    }
}
