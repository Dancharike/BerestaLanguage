//
// Created by Denis on 09.07.2025.
//

#include "Evaluator.h"
#include "Expression.h"
#include "Statement.h"
#include "../interpreter/FunctionIndex.h"
#include "builtin/core/BuiltinRegistry.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>

struct ReturnException {Value value; explicit ReturnException(Value val) : value(std::move(val)) {}};

Evaluator::Evaluator(Environment &env, FunctionIndex &index, std::string current_file, Diagnostics& diagnostics)
    : BaseContext(diagnostics, std::move(current_file)), _env(env), _index(index)
    {
        _file_stack.push_back(std::move(current_file));
    }

Value Evaluator::eval_expression(Expression* expr)
{
    if(!expr) {return {};}
    return expr->accept(*this);
}

Value Evaluator::eval_statement(Statement* stmt)
{
    if(!stmt) {return {};}
    return stmt->accept(*this);
}

bool Evaluator::is_truthy(const Value& val)
{
    switch(val.type)
    {
        case ValueType::BOOLEAN:    {return std::get<bool>(val.data);}
        case ValueType::INTEGER:    {return std::get<int>(val.data) != 0;}
        case ValueType::DOUBLE:     {return std::get<double>(val.data) != 0.0;}
        case ValueType::STRING:     {return !std::get<std::string>(val.data).empty();}
        case ValueType::ARRAY:      {return !std::get<std::vector<Value>>(val.data).empty();}
        case ValueType::DICTIONARY: {return !std::get<DictionaryPtr>(val.data)->empty();}
        case ValueType::NONE:       {return false;}
    }

    return false;
}

void Evaluator::type_error(int line, const char* msg)
{
    _diag.error(msg, current_file(), line);
}

Value Evaluator::visit_number(NumberExpr& expr) {return expr.value;}

Value Evaluator::visit_string(StringExpr& expr) {return Value(expr.value);}

Value Evaluator::visit_bool(BoolExpr& expr) {return Value(expr.value);}

Value Evaluator::visit_variable(VariableExpr& expr) {return _env.get(expr.name);}

Value Evaluator::visit_unary(UnaryExpr& expr)
{
    Value r = eval_expression(expr.right.get());
    switch(expr.op)
    {
        case '+':
        {
            if(r.type == ValueType::INTEGER) {return Value(std::get<int>(r.data));}
            if(r.type == ValueType::DOUBLE) {return Value(std::get<double>(r.data));}
            break;
        }

        case '-':
        {
            if(r.type == ValueType::INTEGER) {return Value(-std::get<int>(r.data));}
            if(r.type == ValueType::DOUBLE) {return Value(-std::get<double>(r.data));}
            break;
        }

        case '!':
        {
            bool b = is_truthy(r);
            return Value(!b);
        }
    }

    type_error(expr.line, "Unsupported unary operand type");
    return {};
}

Value Evaluator::visit_binary(BinaryExpr& expr)
{
    Value lv = eval_expression(expr.left.get());
    Value rv = eval_expression(expr.right.get());
    const std::string& op = expr.op;

    auto both_nums = (lv.type == ValueType::INTEGER || lv.type == ValueType::DOUBLE) && (rv.type == ValueType::INTEGER || rv.type == ValueType::DOUBLE);
    auto as_double = [](const Value& v)->double {return v.type == ValueType::DOUBLE ? std::get<double>(v.data) : static_cast<double>(std::get<int>(v.data));};

    if(both_nums)
    {
        double l = as_double(lv), r = as_double(rv);
        if(op == "+") {return Value(l + r);}
        if(op == "-") {return Value(l - r);}
        if(op == "*") {return Value(l * r);}
        if(op == "/") {return Value(r != 0.0 ? l / r : 0.0);}

        if(op == "==") {return Value(l == r);}
        if(op == "!=") {return Value(l != r);}
        if(op == "<")  {return Value(l < r);}
        if(op == "<=") {return Value(l <= r);}
        if(op == ">")  {return Value(l > r);}
        if(op == ">=") {return Value(l >= r);}
    }

    if(lv.type == ValueType::BOOLEAN && rv.type == ValueType::BOOLEAN)
    {
        bool l = std::get<bool>(lv.data), r = std::get<bool>(rv.data);
        if(op == "==") {return Value(l == r);}
        if(op == "!=") {return Value(l != r);}
        if(op == "and" || op == "&&") {return Value(l && r);}
        if(op == "or"  || op == "||") {return Value(l || r);}
    }

    if(op == "+" && (lv.type == ValueType::STRING || rv.type == ValueType::STRING)) {return Value(lv.to_string() + rv.to_string());}

    type_error(expr.line, "Unsupported operand types for binary operator");
    return {};
}

Value Evaluator::visit_call(FunctionCallExpr& expr)
{
    auto* var = dynamic_cast<VariableExpr*>(expr.callee.get());
    if(!var) {_diag.error("Only simple function names supported (e.g. foo(1))", current_file(), expr.line); return {};}

    std::string fn_name = var->name;
    std::vector<Value> args;
    args.reserve(expr.arguments.size());
    for(auto& a : expr.arguments) {args.push_back(eval_expression(a.get()));}

    if(auto* impl = BuiltinRegistry::instance().get(fn_name))
    {
        try {return impl->invoke(args);}
        catch(const std::exception& ex) {_diag.error(std::string("Builtin error: ") + ex.what(), current_file(), expr.line); return {};}
        catch(...)                      {_diag.error("Builtin error: exception", current_file(), expr.line); return {};}
    }

    const FunctionRef* ref = _index.find_function(fn_name, current_file());
    if(!ref) {_diag.error("Unknown function: " + fn_name, current_file(), expr.line); return {};}

    FunctionStatement* fn = ref->func;
    if(args.size() != fn->parameters.size()) {std::cerr << "[ERROR] Function " << fn_name << " expects " << fn->parameters.size() << " args, got " << args.size() << "\n"; return {};}

    bool pushed_file = false;
    if(ref->is_public && ref->file != current_file())
    {
        set_current_file(ref->file);
        _file_stack.push_back(ref->file);
        pushed_file = true;
    }

    _env.push_scope();
    for(size_t i = 0; i < args.size(); ++i)
    {
        _env.define(fn->parameters[i], args[i]);
    }

    Value result;
    try {result = eval_statement(fn->body.get());}
    catch(const ReturnException& re) {result = re.value;}

    _env.pop_scope();
    if(pushed_file)
    {
        _file_stack.pop_back();
        if(!_file_stack.empty()) {set_current_file(_file_stack.back());}
    }

    return result;
}

Value Evaluator::visit_array(ArrayLiteralExpr& expr)
{
    std::vector<Value> elems;
    elems.reserve(expr.elements.size());
    for(auto& x : expr.elements)
    {
        elems.push_back(eval_expression(x.get()));
    }

    return Value(elems);
}

Value Evaluator::visit_dictionary(DictionaryLiteralExpr& expr)
{
    Dictionary dict;
    for(auto& kv : expr.entries)
    {
        Value k = eval_expression(kv.first.get());
        Value v = eval_expression(kv.second.get());
        if(k.type != ValueType::STRING) {_diag.error("Dictionary key must be string literal", current_file(), expr.line); return {};}
        dict[std::get<std::string>(k.data)] = v;
    }

    return Value(dict);
}

Value Evaluator::visit_index(IndexExpr& expr)
{
    Value container = eval_expression(expr.array.get());
    Value idx = eval_expression(expr.index.get());

    if(container.type == ValueType::ARRAY)
    {
        int i = 0;
        if(idx.type == ValueType::INTEGER)     {i = std::get<int>(idx.data);}
        else if(idx.type == ValueType::DOUBLE) {i = static_cast<int>(std::get<double>(idx.data));}
        else                                   {_diag.error("Array index must be numeric", current_file(), expr.line); return {};}

        auto& arr = std::get<std::vector<Value>>(container.data);
        if(i < 0 || i >= static_cast<int>(arr.size())) {_diag.error("Array index out of bounds", current_file(), expr.line); return {};}
        return arr[i];
    }

    if(container.type == ValueType::DICTIONARY)
    {
        std::string key = (idx.type == ValueType::STRING) ? std::get<std::string>(idx.data) : idx.to_string();
        const auto& dict = *std::get<DictionaryPtr>(container.data);
        auto it = dict.find(key);
        if(it == dict.end()) {_diag.error("Key not found in dictionary: " + key, current_file(), expr.line); return {};}
        return it->second;
    }

    _diag.error("Indexing not supported for this type", current_file(), expr.line);
    return {};
}

Value Evaluator::visit_member(MemberAccessExpr& expr)
{
    _diag.warn("Member access not yet implemented", current_file(), expr.line);
    return {};
}

Value Evaluator::visit_assignment(Assignment& stmt)
{
    Value v = eval_expression(stmt.value.get());
    if(stmt.is_let) {_env.define(stmt.name, v);}
    else            {_env.assign(stmt.name, v);}
    return v;
}

Value Evaluator::visit_assignment_statement(AssignmentStatement& stmt)
{
    return stmt.assignment->accept(*this);
}

Value Evaluator::visit_expr_stmt(ExpressionStatement& stmt)
{
    return eval_expression(stmt.expression.get());
}

Value Evaluator::visit_if(IfStatement& stmt)
{
    if(is_truthy(eval_expression(stmt.condition.get()))) {return eval_statement(stmt.then_branch.get());}
    else if(stmt.else_branch) {return eval_statement(stmt.else_branch.get());}
    return {};
}

Value Evaluator::visit_while(WhileStatement& stmt)
{
    Value result;
    while(is_truthy(eval_expression(stmt.condition.get())))
    {
        result = eval_statement(stmt.body.get());
    }
    return result;
}

Value Evaluator::visit_repeat(RepeatStatement& stmt)
{
    Value cnt = eval_expression(stmt.count.get());
    int n = 0;
    if(cnt.type == ValueType::INTEGER)     {n = std::get<int>(cnt.data);}
    else if(cnt.type == ValueType::DOUBLE) {n = static_cast<int>(std::get<double>(cnt.data));}
    else                                   {_diag.error("repeat() count must be numeric", current_file(), stmt.line); return {};}

    Value result;
    for(int i = 0; i < n; ++i)
    {
        result = eval_statement(stmt.body.get());
    }
    return result;
}

Value Evaluator::visit_for(ForStatement& stmt)
{
    Value result;
    _env.push_scope();
    if(stmt.initializer) {eval_statement(stmt.initializer.get());}

    while(true)
    {
        bool truthy = true;
        if(stmt.condition) {truthy = is_truthy(eval_expression(stmt.condition.get()));}

        if(!truthy) {break;}

        result = eval_statement(stmt.body.get());
        if(stmt.increment) {eval_statement(stmt.increment.get());}
    }

    _env.pop_scope();
    return result;
}

Value Evaluator::visit_foreach(ForeachStatement& stmt)
{
    Value it = eval_expression(stmt.iterable.get());
    if(it.type != ValueType::ARRAY) {_diag.error("foreach() expects an array", current_file(), stmt.line); return {};}
    const auto& arr = std::get<std::vector<Value>>(it.data);
    Value result;
    for(const auto& elem : arr)
    {
        _env.push_scope();
        _env.define(stmt.var_name, elem);
        result = eval_statement(stmt.body.get());
        _env.pop_scope();
    }
    return result;
}

Value Evaluator::visit_block(BlockStatement& stmt)
{
    _env.push_scope();
    Value result;
    for(const auto& st : stmt.statements)
    {
        result = eval_statement(st.get());
    }
    _env.pop_scope();
    return result;
}

// тут ничего не надо, просто указать такой тип, ибо функции уже до этого момента проиндексированы
Value Evaluator::visit_function(FunctionStatement& stmt) {return {};}

Value Evaluator::visit_return(ReturnStatement& stmt)
{
    Value v = stmt.value ? eval_expression(stmt.value.get()) : Value();
    throw ReturnException(v);
}

Value Evaluator::visit_index_assignment(IndexAssignment& stmt)
{
    std::vector<Value> indices;
    Expression* target_expr = stmt.target.get();

    while(auto* idx = dynamic_cast<IndexExpr*>(target_expr))
    {
        indices.push_back(eval_expression(idx->index.get()));
        target_expr = idx->array.get();
    }

    auto* var = dynamic_cast<VariableExpr*>(target_expr);
    if(!var) {_diag.error("Indexed assignment target must be variable", current_file(), stmt.line); return {};}

    Value container = _env.get(var->name);
    Value new_val = eval_expression(stmt.value.get());
    std::reverse(indices.begin(), indices.end());
    Value* cur = &container;

    for(size_t level = 0; level < indices.size(); ++level)
    {
        Value idx_val = indices[level];
        bool last = (level + 1 == indices.size());

        if(cur->type == ValueType::ARRAY)
        {
            int i = (idx_val.type == ValueType::INTEGER) ? std::get<int>(idx_val.data): static_cast<int>(std::get<double>(idx_val.data));
            auto& arr = std::get<std::vector<Value>>(cur->data);
            if(i < 0)                             {_diag.error("Negative array index", current_file(), stmt.line); return {};}
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
                if(it == dict.end()) {dict[key] = Value(Dictionary{}); cur = &dict[key];}
                else                 {cur = &it->second;}
            }
        }
        else {_diag.error("Indexed assignment not supported for this type", current_file(), stmt.line); return {};}
    }

    _env.assign(var->name, container);
    return new_val;
}

Value Evaluator::visit_enum(EnumStatement& stmt)
{
    for(const auto& [name, val] : stmt.members)
    {
        _env.define(stmt.name + "." + name, Value(val));
    }
    return {};
}

/*
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

            _diag.error("Unsupported unary operand type operator", current_file(), expr->line);
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

            _diag.error("Unsupported operand types for binary operator ", current_file(), expr->line);
            return {};
        }

        case ExpressionType::FUNCTION_CALL:
        {
            auto* call = dynamic_cast<FunctionCallExpr*>(expr);
            auto* callee_var = dynamic_cast<VariableExpr*>(call->callee.get());

            if(!callee_var) {_diag.error("Only simple function names supported (e.g. foo(1))", current_file(), expr->line); return {};}

            std::string fn_name = callee_var->name;

            std::vector<Value> args;
            for(auto& arg : call->arguments)
            {
                args.push_back(eval_expression(arg.get()));
            }

            if(auto it = builtin_functions.find(fn_name); it != builtin_functions.end())
            {
                try {return it->second(args);}
                catch(...) {_diag.error("Exception in builtin function: ", current_file(), expr->line); return {};}
            }

            const FunctionRef* ref = _index.find_function(fn_name, current_file());
            if(!ref) {_diag.error("Unknown function: " + fn_name, current_file(), expr->line); return {};}

            FunctionStatement* fn = ref->func;
            if(args.size() != fn->parameters.size()) {std::cerr << "[ERROR] Function " << fn_name << " expects " << fn->parameters.size() << " args, got " << args.size() << "\n"; return {};}

            bool pushed_file = false;
            if(ref->is_public && ref->file != current_file())
            {
                set_current_file(ref->file);
                _file_stack.push_back(ref->file);
                pushed_file = true;
            }

            _env.push_scope();
            for(size_t i = 0; i < args.size(); ++i)
            {
                _env.define(fn->parameters[i], args[i]);
            }

            Value result;

            try {result = eval_statement(fn->body.get());}
            catch(const ReturnException& e) {result = e.value;}

            _env.pop_scope();
            if(pushed_file)
            {
                _file_stack.pop_back();
                if(!_file_stack.empty()) {set_current_file(_file_stack.back());}
            }

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
                if(k.type != ValueType::STRING) {_diag.error("Dictionary key must be string literal", current_file(), expr->line); return {};}
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
                else                                         {_diag.error("Array index must be numeric", current_file(), expr->line); return {};}

                auto& arr = std::get<std::vector<Value>>(container.data);
                if(i < 0 || i >= static_cast<int>(arr.size())) {_diag.error("Array index out of bounds", current_file(), expr->line); return {};}

                return arr[i];
            }

            if(container.type == ValueType::DICTIONARY)
            {
                std::string key;
                if(index_val.type == ValueType::STRING)  {key = std::get<std::string>(index_val.data);}
                else                                     {key = index_val.to_string();}

                const auto &dict = *std::get<DictionaryPtr>(container.data);
                auto it = dict.find(key);
                if(it == dict.end()) {_diag.error("Key not found in dictionary: " + key, current_file(), expr->line); return {};}
                return it->second;
            }
        }

        default: _diag.warn("Expression type not yet implemented!", current_file(), expr->line); return {};
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
            else                                     {_diag.error("Invalid condition type in if-statement", current_file(), stmt->line);}

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
                else                                     {_diag.error("Invalid while() condition type", current_file(), stmt->line);}

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
            else                                         {_diag.error("repeat() count must be numeric", current_file(), stmt->line); return {};}

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
                    else                                     {_diag.error("Invalid for-loop condition type", current_file(), stmt->line); break;}
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

            if(iterable.type != ValueType::ARRAY) {_diag.error("foreach() expects an array", current_file(), stmt->line); return {};}

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
            if(!var) {_diag.error("Indexed assignment target must be variable", current_file(), stmt->line); return {};}

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
                    if(i < 0)                             {_diag.error("Negative array index", current_file(), stmt->line); return {};}
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
                else {_diag.error("Indexed assignment not supported for this type", current_file(), stmt->line); return {};}
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

        default: _diag.warn("Statement type not yet implemented", current_file(), stmt->line); return {};
    }
}
*/