//
// Created by Denis on 09.10.2025.
//

#include "BuiltinMathCore.h"
#include "builtin/core/BuiltinRegistry.h"
#include "builtin/core/BuiltinUtils.h"
#include <cmath>
#include <algorithm>

Value BuiltinSqr::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "sqr")) {return {};}
    if(!check_numeric(diag, file, line, args[0], "sqr", 1)) {return {};}
    double x = num(args[0]);
    return Value(x * x);
}

Value BuiltinSqrt::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "sqrt")) {return {};}
    if(!check_numeric(diag, file, line, args[0], "sqrt", 1)) {return {};}
    double x = num(args[0]);
    if(x < 0.0) {diag.error("sqrt: argument must be non-negative", file, line); return {};}
    return Value(std::sqrt(x));
}

Value BuiltinAbs::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "abs")) {return {};}
    if(!check_numeric(diag, file, line, args[0], "abs", 1)) {return {};}
    return Value(std::fabs(num(args[0])));
}

Value BuiltinRound::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "round")) {return {};}
    if(!check_numeric(diag, file, line, args[0], "round", 1)) {return {};}
    return Value(std::round(num(args[0])));
}

Value BuiltinFloor::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "floor")) {return {};}
    if(!check_numeric(diag, file, line, args[0], "floor", 1)) {return {};}
    return Value(std::floor(num(args[0])));
}

Value BuiltinCeil::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "ceil")) {return {};}
    if(!check_numeric(diag, file, line, args[0], "ceil", 1)) {return {};}
    return Value(std::ceil(num(args[0])));
}

Value BuiltinFrac::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "frac")) {return {};}
    if(!check_numeric(diag, file, line, args[0], "frac", 1)) {return {};}
    double x = num(args[0]);
    return Value(x - std::floor(x));
}

Value BuiltinPower::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 2, "power")) {return {};}
    if(!check_numeric(diag, file, line, args[0], "power", 1)) {return {};}
    if(!check_numeric(diag, file, line, args[1], "power", 2)) {return {};}
    return Value(std::pow(num(args[0]), num(args[1])));
}

Value BuiltinClamp::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 3, "clamp")) {return {};}
    if(!check_numeric(diag, file, line, args[0], "clamp", 1) || !check_numeric(diag, file, line, args[1], "clamp", 2) || !check_numeric(diag, file, line, args[2], "clamp", 3)) {return {};}
    double v = num(args[0]);
    double mn = num(args[1]);
    double mx = num(args[2]);
    return Value(std::clamp(v, mn, mx));
}

Value BuiltinLerp::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 3, "lerp")) {return {};}
    if(args[0].type != ValueType::DOUBLE || args[1].type != ValueType::DOUBLE || args[2].type != ValueType::DOUBLE) {diag.error("lerp expects 3 double arguments (a, b, amount)", file, line); return {};}
    double a = std::get<double>(args[0].data);
    double b = std::get<double>(args[1].data);
    double amt = std::get<double>(args[2].data);
    return Value(a + (b - a) * amt);
}

Value BuiltinMin::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 2, "min")) {return {};}
    if(!check_numeric(diag, file, line, args[0], "min", 1) || !check_numeric(diag, file, line, args[1], "min", 2)) {return {};}
    return Value(std::min(num(args[0]), num(args[1])));
}

Value BuiltinMax::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 2, "max")) {return {};}
    if(!check_numeric(diag, file, line, args[0], "max", 1) || !check_numeric(diag, file, line, args[1], "max", 2)) {return {};}
    return Value(std::max(num(args[0]), num(args[1])));
}

Value BuiltinMean::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity_range(diag, file, line, args, 1, "mean")) {return {};}
    double sum = 0.0;
    for(size_t i = 0; i < args.size(); ++i)
    {
        if(!check_numeric(diag, file, line, args[i], "mean", static_cast<int>(i + 1))) {return {};}
        sum += num(args[i]);
    }
    return Value(sum / static_cast<double>(args.size()));
}

Value BuiltinMedian::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity_range(diag, file, line, args, 1, "median")) {return {};}
    std::vector<double> vals;
    vals.reserve(args.size());
    for(size_t i = 0; i < args.size(); ++i)
    {
        if(!check_numeric(diag, file, line, args[i], "median", static_cast<int>(i + 1))) {return {};}
        vals.push_back(num(args[i]));
    }
    std::sort(vals.begin(), vals.end());
    size_t mid = vals.size() / 2;
    return Value(vals[mid]);
}

Value BuiltinLn::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "ln")) {return {};}
    if(!check_numeric(diag, file, line, args[0], "ln", 1)) {return {};}
    double x = num(args[0]);
    if(x <= 0.0) {diag.error("ln: input must be > 0", file, line); return {};}
    return Value(std::log(x));
}

Value BuiltinLog2::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "log2")) {return {};}
    if(!check_numeric(diag, file, line, args[0], "log2", 1)) {return {};}
    double x = num(args[0]);
    if(x <= 0.0) {diag.error("log2 domain error: input must be positive", file, line); return {};}
    return Value(std::log2(x));
}

Value BuiltinLog10::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "log10")) {return {};}
    if(!check_numeric(diag, file, line, args[0], "log10", 1)) {return {};}
    double x = num(args[0]);
    if(x <= 0.0) {diag.error("log10 domain error: input must be positive", file, line); return {};}
    return Value(std::log10(x));
}

Value BuiltinLogn::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 2, "logn")) {return {};}
    if(!check_numeric(diag, file, line, args[0], "logn", 1) || !check_numeric(diag, file, line, args[1], "logn", 2)) {return {};}
    double base = num(args[0]);
    double val  = num(args[1]);
    if(base <= 0.0 || base == 1.0 || val <= 0.0) {diag.error("logn domain error: base must be > 0 and ≠ 1, value must be > 0", file, line); return {};}
    return Value(std::log(val) / std::log(base));
}

void register_builtin_math_core()
{
    auto& reg = BuiltinRegistry::instance();
    reg.register_builtin(std::make_unique<BuiltinSqr>());
    reg.register_builtin(std::make_unique<BuiltinSqrt>());
    reg.register_builtin(std::make_unique<BuiltinAbs>());
    reg.register_builtin(std::make_unique<BuiltinRound>());
    reg.register_builtin(std::make_unique<BuiltinFloor>());
    reg.register_builtin(std::make_unique<BuiltinCeil>());
    reg.register_builtin(std::make_unique<BuiltinFrac>());
    reg.register_builtin(std::make_unique<BuiltinPower>());
    reg.register_builtin(std::make_unique<BuiltinClamp>());
    reg.register_builtin(std::make_unique<BuiltinLerp>());
    reg.register_builtin(std::make_unique<BuiltinMin>());
    reg.register_builtin(std::make_unique<BuiltinMax>());
    reg.register_builtin(std::make_unique<BuiltinMean>());
    reg.register_builtin(std::make_unique<BuiltinMedian>());
    reg.register_builtin(std::make_unique<BuiltinLn>());
    reg.register_builtin(std::make_unique<BuiltinLog2>());
    reg.register_builtin(std::make_unique<BuiltinLog10>());
    reg.register_builtin(std::make_unique<BuiltinLogn>());
}
