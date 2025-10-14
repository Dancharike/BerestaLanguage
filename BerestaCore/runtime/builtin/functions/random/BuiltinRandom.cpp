//
// Created by Denis on 09.10.2025.
//

#include "BuiltinRandom.h"
#include "runtime/builtin/core/BuiltinRegistry.h"
#include "runtime/builtin/core/BuiltinUtils.h"
#include <random>
#include <cmath>
#include <algorithm>

static std::mt19937& rng()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}

Value BuiltinChooseFrom::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(args.empty()) {diag.error("choose_from expects at least 1 argument", file, line); return {};}
    std::uniform_int_distribution<size_t> dist(0, args.size() - 1);
    return args[dist(rng())];
}


Value BuiltinRandom::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "random") || !check_numeric(diag, file, line, args[0], "random", 1)) {return {};}
    double upper = num(args[0]);
    if(upper <= 0.0) {diag.error("random upper bound must be > 0", file, line); return {};}
    std::uniform_real_distribution<double> dist(0.0, upper);
    return Value(dist(rng()));
}

Value BuiltinRandomRange::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 2, "random_range") || !check_numeric(diag, file, line, args[0], "random_range", 1) || !check_numeric(diag, file, line, args[1], "random_range", 2)) {return {};}
    double min = num(args[0]);
    double max = num(args[1]);
    if(min > max) {std::swap(min, max);}
    std::uniform_real_distribution<double> dist(min, max);
    return Value(dist(rng()));
}

Value BuiltinIntRandom::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 1, "int_random") || !check_numeric(diag, file, line, args[0], "int_random", 1)) {return {};}
    int upper = as_int_lossy(args[0]);
    if(upper < 0) {diag.error("int_random: upper bound must be >= 0", file, line); return {};}
    std::uniform_int_distribution<int> dist(0, upper);
    return Value(dist(rng()));
}

Value BuiltinIntRandomRange::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    if(!check_arity(diag, file, line, args, 2, "int_random_range") || !check_numeric(diag, file, line, args[0], "int_random_range", 1) || !check_numeric(diag, file, line, args[1], "int_random_range", 2)) {return {};}
    int min_val = as_int_lossy(args[0]);
    int max_val = as_int_lossy(args[1]);
    if(min_val > max_val) {std::swap(min_val, max_val);}
    std::uniform_int_distribution<int> dist(min_val, max_val);
    return Value(dist(rng()));
}

void register_builtin_random()
{
    auto& reg = BuiltinRegistry::instance();
    reg.register_builtin(std::make_unique<BuiltinChooseFrom>());
    reg.register_builtin(std::make_unique<BuiltinRandom>());
    reg.register_builtin(std::make_unique<BuiltinRandomRange>());
    reg.register_builtin(std::make_unique<BuiltinIntRandom>());
    reg.register_builtin(std::make_unique<BuiltinIntRandomRange>());
}
