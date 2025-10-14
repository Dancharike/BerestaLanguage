//
// Created by Denis on 09.10.2025.
//

#include "BuiltinMathTrig.h"
#include "runtime/builtin/core/BuiltinRegistry.h"
#include "runtime/builtin/core/BuiltinUtils.h"
#include <cmath>

static constexpr double DEGTORAD = M_PI / 180.0;
static constexpr double RADTODEG = 180.0 / M_PI;

Value BuiltinSin::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "sin") || !check_numeric(diag, file, line, args[0], "sin", 1)) {return {};}
    return Value(std::sin(num(args[0])));
}

Value BuiltinCos::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "cos") || !check_numeric(diag, file, line, args[0], "cos", 1)) {return {};}
    return Value(std::cos(num(args[0])));
}

Value BuiltinTan::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "tan") || !check_numeric(diag, file, line, args[0], "tan", 1)) {return {};}
    double radians = num(args[0]);
    double result = std::tan(radians);
    if(std::abs(result) < 1e-5) {result = 0.0;}
    if(!std::isfinite(result)) {diag.error("tan argument results in infinity (asymptote)", file, line); return {};}
    return Value(result);
}

Value BuiltinArcsin::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "arcsin") || !check_numeric(diag, file, line, args[0], "arcsin", 1)) {return {};}
    double v = num(args[0]);
    if(v < -1.0 || v > 1.0) {diag.error("arcsin domain error: input must be in range [-1, 1]", file, line); return {};}
    return Value(std::asin(v));
}

Value BuiltinArccos::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "arccos") || !check_numeric(diag, file, line, args[0], "arccos", 1)) {return {};}
    double v = num(args[0]);
    if(v < -1.0 || v > 1.0) {diag.error("arccos domain error: input must be in range [-1, 1]", file, line); return {};}
    return Value(std::acos(v));
}

Value BuiltinArctan::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "arctan") || !check_numeric(diag, file, line, args[0], "arctan", 1)) {return {};}
    return Value(std::atan(num(args[0])));
}

Value BuiltinArctan2::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 2, "arctan2") || !check_numeric(diag, file, line, args[0], "arctan2", 1) || !check_numeric(diag, file, line, args[1], "arctan2", 2)) {return {};}
    double y = num(args[0]);
    double x = num(args[1]);
    double result = std::atan2(y, x);
    result = std::round(result * 100.0) / 100.0;
    return Value(result);
}

Value BuiltinDsin::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "dsin") || !check_numeric(diag, file, line, args[0], "dsin", 1)) {return {};}
    double radians = num(args[0]) * DEGTORAD;
    double result = std::sin(radians);
    if(std::abs(result) < 1e-5) {result = 0.0;}
    result = std::round(result * 100.0) / 100.0;
    return Value(result);
}

Value BuiltinDcos::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "dcos") || !check_numeric(diag, file, line, args[0], "dcos", 1)) {return {};}
    double radians = num(args[0]) * DEGTORAD;
    double result = std::cos(radians);
    if(std::abs(result) < 1e-5) {result = 0.0;}
    result = std::round(result * 100.0) / 100.0;
    return Value(result);
}

Value BuiltinDtan::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "dtan") || !check_numeric(diag, file, line, args[0], "dtan", 1)) {return {};}
    double degrees = num(args[0]);
    double radians = degrees * DEGTORAD;
    double result = std::tan(radians);
    if(std::fabs(std::fmod(std::abs(degrees), 180.0) - 90.0) < 1e-6) {diag.error("dtan asymptote at " + std::to_string(degrees) + " degrees", file, line); return {};}
    if(!std::isfinite(result)) {diag.error("dtan argument results in infinity (asymptote)", file, line); return {};}
    if(std::abs(result) < 1e-5) {result = 0.0;}
    result = std::round(result * 100.0) / 100.0;
    return Value(result);
}

Value BuiltinDarcsin::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "darcsin") || !check_numeric(diag, file, line, args[0], "darcsin", 1)) {return {};}
    double v = num(args[0]);
    if(v < -1.0 || v > 1.0) {diag.error("darcsin domain error: input must be in range [-1, 1]", file, line); return {};}
    double degrees = std::asin(v) * RADTODEG;
    degrees = std::round(degrees * 100.0) / 100.0;
    return Value(degrees);
}

Value BuiltinDarccos::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "darccos") || !check_numeric(diag, file, line, args[0], "darccos", 1)) {return {};}
    double v = num(args[0]);
    if(v < -1.0 || v > 1.0) {diag.error("darccos domain error: input must be in range [-1, 1]", file, line); return {};}
    double degrees = std::acos(v) * RADTODEG;
    degrees = std::round(degrees * 100.0) / 100.0;
    return Value(degrees);
}

Value BuiltinDarctan::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "darctan") || !check_numeric(diag, file, line, args[0], "darctan", 1)) {return {};}
    double degrees = std::atan(num(args[0])) * RADTODEG;
    degrees = std::round(degrees * 100.0) / 100.0;
    return Value(degrees);
}

Value BuiltinDarctan2::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 2, "darctan2") || !check_numeric(diag, file, line, args[0], "darctan2", 1) || !check_numeric(diag, file, line, args[1], "darctan2", 2)) {return {};}
    double y = num(args[0]);
    double x = num(args[1]);
    double degrees = std::atan2(y, x) * RADTODEG;
    degrees = std::round(degrees * 100.0) / 100.0;
    return Value(degrees);
}

Value BuiltinPointDirection::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 4, "point_direction")) {return {};}
    for(int i = 0; i < 4; ++i)
    {
        if(!check_numeric(diag, file, line, args[i], "point_direction", i + 1)) {return {};}
    }
    double x1 = num(args[0]), y1 = num(args[1]), x2 = num(args[2]), y2 = num(args[3]);
    double radians = std::atan2(y2 - y1, x2 - x1);
    double degrees = radians * RADTODEG;
    degrees = std::round(degrees * 100.0) / 100.0;
    return Value(degrees);
}

Value BuiltinPointDistance::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 4, "point_distance")) {return {};}
    for(int i = 0; i < 4; ++i)
    {
        if(!check_numeric(diag, file, line, args[i], "point_distance", i + 1)) {return {};}
    }
    double x1 = num(args[0]), y1 = num(args[1]), x2 = num(args[2]), y2 = num(args[3]);
    double distance = std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    distance = std::round(distance * 100.0) / 100.0;
    return Value(distance);
}

Value BuiltinLengthdirX::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 2, "lengthdir_x") || !check_numeric(diag, file, line, args[0], "lengthdir_x", 1) || !check_numeric(diag, file, line, args[1], "lengthdir_x", 2)) {return {};}
    double len = num(args[0]);
    double dir = num(args[1]) * DEGTORAD;
    return Value(len * std::cos(dir));
}

Value BuiltinLengthdirY::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 2, "lengthdir_y") || !check_numeric(diag, file, line, args[0], "lengthdir_y", 1) || !check_numeric(diag, file, line, args[1], "lengthdir_y", 2)) {return {};}
    double len = num(args[0]);
    double dir = num(args[1]) * DEGTORAD;
    return Value(-len * std::sin(dir));
}

void register_builtin_math_trig()
{
    auto& reg = BuiltinRegistry::instance();
    reg.register_builtin(std::make_unique<BuiltinSin>());
    reg.register_builtin(std::make_unique<BuiltinCos>());
    reg.register_builtin(std::make_unique<BuiltinTan>());
    reg.register_builtin(std::make_unique<BuiltinArcsin>());
    reg.register_builtin(std::make_unique<BuiltinArccos>());
    reg.register_builtin(std::make_unique<BuiltinArctan>());
    reg.register_builtin(std::make_unique<BuiltinArctan2>());
    reg.register_builtin(std::make_unique<BuiltinDsin>());
    reg.register_builtin(std::make_unique<BuiltinDcos>());
    reg.register_builtin(std::make_unique<BuiltinDtan>());
    reg.register_builtin(std::make_unique<BuiltinDarcsin>());
    reg.register_builtin(std::make_unique<BuiltinDarccos>());
    reg.register_builtin(std::make_unique<BuiltinDarctan>());
    reg.register_builtin(std::make_unique<BuiltinDarctan2>());
    reg.register_builtin(std::make_unique<BuiltinPointDirection>());
    reg.register_builtin(std::make_unique<BuiltinPointDistance>());
    reg.register_builtin(std::make_unique<BuiltinLengthdirX>());
    reg.register_builtin(std::make_unique<BuiltinLengthdirY>());
}
