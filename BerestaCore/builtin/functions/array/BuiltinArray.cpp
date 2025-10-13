//
// Created by Denis on 09.10.2025.
//

#include "BuiltinArray.h"
#include "builtin/core/BuiltinRegistry.h"
#include "builtin/core/BuiltinUtils.h"
#include <algorithm>
#include <random>
#include <cmath>

static bool ensure_array_arg(Diagnostics& diag, const std::string& file, int line, const std::vector<Value>& args, size_t idx, const std::string& name)
{
    if(idx >= args.size()) {diag.error(name + ": missing argument #" + std::to_string(idx), file, line); return false;}
    if(args[idx].type != ValueType::ARRAY) {diag.error(name + ": argument #" + std::to_string(idx) + " must be array", file, line); return false;}
    return true;
}

static bool to_index_nonneg(const Value& v, size_t& out)
{
    long long i = 0;
    if(v.type == ValueType::INTEGER) {i = static_cast<long long>(std::get<int>(v.data));}
    else if(v.type == ValueType::DOUBLE) {i = static_cast<long long>(std::floor(std::get<double>(v.data)));}
    else {return false;}
    if(i < 0) {return false;}
    out = static_cast<size_t>(i);
    return true;
}

static bool value_equals(const Value& a, const Value& b)
{
    if(a.type != b.type) return false;
    switch(a.type)
    {
        case ValueType::INTEGER: {return std::get<int>(a.data) == std::get<int>(b.data);}
        case ValueType::DOUBLE:  {return std::get<double>(a.data) == std::get<double>(b.data);}
        case ValueType::BOOLEAN: {return std::get<bool>(a.data) == std::get<bool>(b.data);}
        case ValueType::STRING:  {return std::get<std::string>(a.data) == std::get<std::string>(b.data);}
        case ValueType::ARRAY:
        {
            const auto& va = std::get<std::vector<Value>>(a.data);
            const auto& vb = std::get<std::vector<Value>>(b.data);
            if(va.size() != vb.size()) return false;
            for(size_t i = 0; i < va.size(); ++i)
            {
                if(!value_equals(va[i], vb[i])) {return false;}
            }
            return true;
        }
        case ValueType::NONE: {return true;}
        default: {return false;}
    }
}

static bool value_less(const Value& a, const Value& b)
{
    if(a.type == ValueType::INTEGER && b.type == ValueType::INTEGER) {return std::get<int>(a.data) < std::get<int>(b.data);}
    if(a.type == ValueType::DOUBLE && b.type == ValueType::DOUBLE) {return std::get<double>(a.data) < std::get<double>(b.data);}
    if(a.type == ValueType::STRING && b.type == ValueType::STRING) {return std::get<std::string>(a.data) < std::get<std::string>(b.data);}
    return a.type < b.type;
}

Value BuiltinArrayIsArray::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 1, 1);
    return Value(args[0].type == ValueType::ARRAY);
}

Value BuiltinArrayLength::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 1, 1);
    if(!ensure_array_arg(diag, file, line, args, 0, name())) {return {};}
    return Value(static_cast<int>(std::get<std::vector<Value>>(args[0].data).size()));
}

Value BuiltinArrayGet::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 2, 2);
    if(!ensure_array_arg(diag, file, line, args, 0, name())) {return {};}
    size_t i = 0;
    if(!to_index_nonneg(args[1], i)) {diag.error("array_get: index must be non-negative", file, line); return {};}
    const auto& v = std::get<std::vector<Value>>(args[0].data);
    if(i >= v.size()) {diag.error("array_get: index out of bounds", file, line); return {};}
    return v[i];
}

Value BuiltinArraySet::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_min_arity(diag, file, line, args, 3);
    if(!ensure_array_arg(diag, file, line, args, 0, name())) {return {};}
    size_t i = 0;
    if(!to_index_nonneg(args[1], i)) {diag.error("array_set: index must be non-negative", file, line); return {};}
    auto v = std::get<std::vector<Value>>(args[0].data);
    if(i >= v.size()) {v.resize(i + 1, Value());}
    v[i] = args[2];
    return Value(v);
}

Value BuiltinArrayPush::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_min_arity(diag, file, line, args, 2);
    if(!ensure_array_arg(diag, file, line, args, 0, name())) {return {};}
    auto v = std::get<std::vector<Value>>(args[0].data);
    v.push_back(args[1]);
    return Value(v);
}

Value BuiltinArrayLast::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 1, 1);
    if(!ensure_array_arg(diag, file, line, args, 0, name())) {return {};}
    const auto& v = std::get<std::vector<Value>>(args[0].data);
    if(v.empty()) {diag.error("array_last: empty array", file, line); return {};}
    return v.back();
}

Value BuiltinArrayPop::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 1, 1);
    if(!ensure_array_arg(diag, file, line, args, 0, name())) {return {};}
    auto v = std::get<std::vector<Value>>(args[0].data);
    if(v.empty()) {diag.error("array_pop: empty array", file, line); return Value(v);}
    v.pop_back();
    return Value(v);
}

Value BuiltinArrayInsert::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_min_arity(diag, file, line, args, 3);
    if(!ensure_array_arg(diag, file, line, args, 0, name())) {return {};}
    size_t i = 0;
    if(!to_index_nonneg(args[1], i)) {diag.error("array_insert: index must be non-negative", file, line); return {};}
    auto v = std::get<std::vector<Value>>(args[0].data);
    if(i > v.size()) {i = v.size();}
    v.insert(v.begin() + static_cast<std::ptrdiff_t>(i), args[2]);
    return Value(v);
}

Value BuiltinArrayDelete::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_min_arity(diag, file, line, args, 2);
    if(!ensure_array_arg(diag, file, line, args, 0, name())) {return {};}
    size_t i = 0;
    if(!to_index_nonneg(args[1], i)) {diag.error("array_delete: index must be non-negative", file, line); return {};}
    auto v = std::get<std::vector<Value>>(args[0].data);
    if(i >= v.size()) {diag.error("array_delete: index out of bounds", file, line); return Value(v);}
    v.erase(v.begin() + static_cast<std::ptrdiff_t>(i));
    return Value(v);
}

Value BuiltinArraySlice::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_min_arity(diag, file, line, args, 2);
    if(!ensure_array_arg(diag, file, line, args, 0, name())) {return {};}
    const auto& src = std::get<std::vector<Value>>(args[0].data);
    size_t start = 0;
    if(!to_index_nonneg(args[1], start)) {diag.error("array_slice: start must be non-negative", file, line); return {};}
    if(start >= src.size()) {return Value(std::vector<Value>{});}
    size_t count = src.size() - start;
    if(args.size() >= 3)
    {
        size_t tmp = 0;
        if(!to_index_nonneg(args[2], tmp)) {diag.error("array_slice: count must be non-negative", file, line); return {};}
        count = tmp;
    }
    size_t avail = src.size() - start;
    size_t take = std::min(count, avail);
    std::vector<Value> out;
    out.reserve(take);
    for(size_t k = 0; k < take; ++k)
    {
        out.push_back(src[start + k]);
    }
    return Value(out);
}

Value BuiltinArrayConcat::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 2, 2);
    if(!ensure_array_arg(diag, file, line, args, 0, name())) {return {};}
    if(!ensure_array_arg(diag, file, line, args, 1, name())) {return {};}
    auto a = std::get<std::vector<Value>>(args[0].data);
    const auto& b = std::get<std::vector<Value>>(args[1].data);
    a.insert(a.end(), b.begin(), b.end());
    return Value(a);
}

Value BuiltinArrayReverse::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 1, 1);
    if(!ensure_array_arg(diag, file, line, args, 0, name())) {return {};}
    auto v = std::get<std::vector<Value>>(args[0].data);
    std::reverse(v.begin(), v.end());
    return Value(v);
}

Value BuiltinArrayIndexOf::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_min_arity(diag, file, line, args, 2);
    if(!ensure_array_arg(diag, file, line, args, 0, name())) {return {};}
    const auto& v = std::get<std::vector<Value>>(args[0].data);
    for(size_t i = 0; i < v.size(); ++i)
    {
        if(value_equals(v[i], args[1])) {return Value(static_cast<int>(i));}
    }
    return Value(-1);
}

Value BuiltinArrayContains::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_min_arity(diag, file, line, args, 2);
    if(!ensure_array_arg(diag, file, line, args, 0, name())) {return {};}
    const auto& v = std::get<std::vector<Value>>(args[0].data);
    for(const auto& e : v)
    {
        if(value_equals(e, args[1])) {return Value(true);}
    }
    return Value(false);
}

Value BuiltinArrayJoin::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_min_arity(diag, file, line, args, 1);
    if(!ensure_array_arg(diag, file, line, args, 0, name())) {return {};}
    std::string sep = (args.size() >= 2 && args[1].type == ValueType::STRING) ? std::get<std::string>(args[1].data) : ",";
    const auto& v = std::get<std::vector<Value>>(args[0].data);
    std::string out;
    for(size_t i = 0; i < v.size(); ++i)
    {
        if(i) out += sep;
        out += v[i].to_string();
    }
    return Value(out);
}

Value BuiltinArrayResize::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_min_arity(diag, file, line, args, 2);
    if(!ensure_array_arg(diag, file, line, args, 0, name())) {return {};}
    size_t n = 0;
    if(!to_index_nonneg(args[1], n)) {diag.error("array_resize: new_size must be non-negative", file, line); return {};}
    auto v = std::get<std::vector<Value>>(args[0].data);
    Value fill = (args.size() >= 3) ? args[2] : Value();
    v.resize(n, fill);
    return Value(v);
}

Value BuiltinArrayFill::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_min_arity(diag, file, line, args, 2);
    size_t n = 0;
    if(!to_index_nonneg(args[0], n)) {diag.error("array_fill: length must be non-negative", file, line); return {};}
    std::vector<Value> v(n, args[1]);
    return Value(v);
}

Value BuiltinArraySort::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_min_arity(diag, file, line, args, 1);
    if(!ensure_array_arg(diag, file, line, args, 0, name())) {return {};}
    bool asc = true;
    if(args.size() >= 2 && args[1].type == ValueType::BOOLEAN) {asc = std::get<bool>(args[1].data);}
    auto v = std::get<std::vector<Value>>(args[0].data);
    std::stable_sort(v.begin(), v.end(), [&](const Value& x, const Value& y)
    {
        return asc ? value_less(x, y) : value_less(y, x);
    });
    return Value(v);
}

Value BuiltinArrayShuffle::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 1, 1);
    if(!ensure_array_arg(diag, file, line, args, 0, name())) {return {};}
    auto v = std::get<std::vector<Value>>(args[0].data);
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::shuffle(v.begin(), v.end(), gen);
    return Value(v);
}

void register_builtin_array()
{
    auto& reg = BuiltinRegistry::instance();
    reg.register_builtin(std::make_unique<BuiltinArrayIsArray>());
    reg.register_builtin(std::make_unique<BuiltinArrayLength>());
    reg.register_builtin(std::make_unique<BuiltinArrayGet>());
    reg.register_builtin(std::make_unique<BuiltinArraySet>());
    reg.register_builtin(std::make_unique<BuiltinArrayPush>());
    reg.register_builtin(std::make_unique<BuiltinArrayLast>());
    reg.register_builtin(std::make_unique<BuiltinArrayPop>());
    reg.register_builtin(std::make_unique<BuiltinArrayInsert>());
    reg.register_builtin(std::make_unique<BuiltinArrayDelete>());
    reg.register_builtin(std::make_unique<BuiltinArraySlice>());
    reg.register_builtin(std::make_unique<BuiltinArrayConcat>());
    reg.register_builtin(std::make_unique<BuiltinArrayReverse>());
    reg.register_builtin(std::make_unique<BuiltinArrayIndexOf>());
    reg.register_builtin(std::make_unique<BuiltinArrayContains>());
    reg.register_builtin(std::make_unique<BuiltinArrayJoin>());
    reg.register_builtin(std::make_unique<BuiltinArrayResize>());
    reg.register_builtin(std::make_unique<BuiltinArrayFill>());
    reg.register_builtin(std::make_unique<BuiltinArraySort>());
    reg.register_builtin(std::make_unique<BuiltinArrayShuffle>());
}
