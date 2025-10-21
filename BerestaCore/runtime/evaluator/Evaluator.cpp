//
// Created by Denis on 09.07.2025.
//

#include "Evaluator.h"
#include "frontend/parser/Expression.h"
#include "frontend/parser/Statement.h"
#include "interpreter/FunctionIndex.h"
#include "runtime/builtin/core/BuiltinRegistry.h"
#include "runtime/value/StructValue.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>

struct ReturnException {Value value; explicit ReturnException(Value val) : value(std::move(val)) {}};

Evaluator::Evaluator(Environment &env, FunctionIndex &index, std::string current_file, Diagnostics& diagnostics)
    : BaseContext(diagnostics, std::move(current_file)), _env(env), _index(index)
    {
        _file_stack.push_back(_current_file);
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

Value Evaluator::visit_variable(VariableExpr& expr) {return _env.get(expr.name, current_file(), expr.line);}

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
    if(auto* var = dynamic_cast<VariableExpr*>(expr.callee.get()))
    {
        std::string fn_name = var->name;

        if(auto* impl = BuiltinRegistry::instance().get(fn_name))
        {
            std::vector<Value> args; args.reserve(expr.arguments.size());
            for(auto& a : expr.arguments)
            {
                args.push_back(eval_expression(a.get()));
            }

            try                             {return impl->invoke(args, _diag, current_file(), expr.line);}
            catch(const std::exception& ex) {_diag.error(std::string("Builtin error: ") + ex.what(), current_file(), expr.line); return {};}
            catch(...)                      {_diag.error("Builtin error: exception", current_file(), expr.line); return {};}
        }

        if(const FunctionRef* ref = _index.find_function(fn_name, current_file()))
        {
            std::vector<Value> args; args.reserve(expr.arguments.size());
            for(auto& a : expr.arguments)
            {
                args.push_back(eval_expression(a.get()));
            }

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
            try                              {result = eval_statement(fn->body.get());}
            catch(const ReturnException& re) {result = re.value;}

            _env.pop_scope();
            if(pushed_file)
            {
                _file_stack.pop_back();
                if(!_file_stack.empty()) {set_current_file(_file_stack.back());}
            }

            return result;
        }
    }

    Value callee_val = eval_expression(expr.callee.get());
    if(callee_val.type == ValueType::STRUCT)
    {
        auto tmpl = std::get<std::shared_ptr<StructInstance>>(callee_val.data);

        auto inst = std::make_shared<StructInstance>();
        inst->definition = tmpl->definition;
        inst->fields     = tmpl->fields;

        const auto& order = inst->definition->field_names;
        size_t n = std::min(order.size(), expr.arguments.size());
        for(size_t i = 0; i < n; ++i)
        {
            Value v = eval_expression(expr.arguments[i].get());
            inst->fields[order[i]] = v;
        }

        return Value(*inst);
    }

    _diag.error("Callee is not a function or struct template", current_file(), expr.line);
    return {};
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
/*
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
*/

Value Evaluator::visit_dictionary(DictionaryLiteralExpr& expr)
{
    _diag.error("Dictionary literals are disabled in this build", current_file(), expr.line);
    return {};
}

Value Evaluator::visit_struct(StructLiteralExpr& expr)
{
    auto def = std::make_shared<StructDefinition>();
    auto inst = std::make_shared<StructInstance>();

    def->field_names.reserve(expr.fields.size());
    for(auto& kv : expr.fields)
    {
        const std::string& name = kv.first;
        def->field_names.push_back(name);
    }

    inst->definition = def;

    for(const auto& name : def->field_names)
    {
        inst->fields[name] = Value();
    }

    return Value(*inst);
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
    /*
    if(container.type == ValueType::DICTIONARY)
    {
        std::string key = (idx.type == ValueType::STRING) ? std::get<std::string>(idx.data) : idx.to_string();
        const auto& dict = *std::get<DictionaryPtr>(container.data);
        auto it = dict.find(key);
        if(it == dict.end()) {_diag.error("Key not found in dictionary: " + key, current_file(), expr.line); return {};}
        return it->second;
    }
    */
    _diag.error("Indexing not supported for this type", current_file(), expr.line);
    return {};
}

Value Evaluator::visit_member(MemberAccessExpr& expr)
{
    Value obj_val = eval_expression(expr.object.get());
    if(obj_val.type == ValueType::STRUCT)
    {
        auto inst = std::get<std::shared_ptr<StructInstance>>(obj_val.data);
        auto it = inst->fields.find(expr.member);
        if(it == inst->fields.end()) {_diag.error("Unknown struct field: " + expr.member, current_file(), expr.line); return {};}
        return it->second;
    }

    std::string base_name;
    if(auto* var = dynamic_cast<VariableExpr*>(expr.object.get())) {base_name = var->name;}
    else {base_name = obj_val.to_string();}

    std::string full_name = base_name + "." + expr.member;

    try        {return _env.get(full_name, current_file(), expr.line);}
    catch(...) {_diag.error("Member access failed: " + full_name, current_file(), expr.line); return {};}
}

Value Evaluator::visit_assignment(Assignment& stmt)
{
    Value v = eval_expression(stmt.value.get());
    if(stmt.is_let) {_env.define(stmt.name, v);}
    else            {_env.assign(stmt.name, v, current_file(), stmt.line);}
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
            int i = (idx_val.type == ValueType::INTEGER) ? std::get<int>(idx_val.data) : static_cast<int>(std::get<double>(idx_val.data));
            auto& arr = std::get<std::vector<Value>>(cur->data);
            if(i < 0)                             {_diag.error("Negative array index", current_file(), stmt.line); return {};}
            if(i >= static_cast<int>(arr.size())) {arr.resize(i + 1, Value());}
            if(last)                              {arr[i] = new_val;}
            else
            {
                if(arr[i].type == ValueType::NONE) {arr[i] = Value(std::vector<Value>{});}
                cur = &arr[i];
            }
        }
        /*
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
        */
        else {_diag.error("Indexed assignment not supported for this type", current_file(), stmt.line); return {};}
    }

    _env.assign(var->name, container, current_file(), stmt.line);
    return new_val;
}

Value Evaluator::visit_enum(EnumStatement& stmt)
{
    _env.define_global(stmt.name, Value());

    for(const auto& [name, val] : stmt.members)
    {
        _env.define_global(stmt.name + "." + name, Value(val));
    }
    return {};
}

Value Evaluator::visit_macros(MacrosStatement& stmt)
{
    Value v = eval_expression(stmt.value.get());

    if(_env.exists(stmt.name)) {_diag.error("Macros already defined" + stmt.name, current_file(), stmt.line); return {};}

    _env.define_global(stmt.name, v);
    return v;
}
