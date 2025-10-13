//
// Created by Denis on 09.10.2025.
//

#include "BuiltinMatrixCore.h"
#include "builtin/core/BuiltinRegistry.h"
#include "builtin/core/BuiltinUtils.h"
#include "builtin/core/Matrix2D.h"
#include "diagnostics/Diagnostics.h"
#include <cmath>
#include <array>

static Matrix2D current_matrix = Matrix2D::identity();

static double get_number(Diagnostics& diag, const std::string& file, int line, const Value& v, const std::string& func_name, size_t index)
{
    if(v.type == ValueType::DOUBLE) {return std::get<double>(v.data);}
    if(v.type == ValueType::INTEGER) {return static_cast<double>(std::get<int>(v.data));}
    diag.error(func_name + ": argument " + std::to_string(index) + " must be numeric", file, line);
    throw std::runtime_error("non-numeric argument");
}

Value BuiltinMatrixGet::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 0, 0);
    std::vector<Value> result;
    for(double d : current_matrix.data)
    {
        result.emplace_back(d);
    }
    return Value(result);
}

Value BuiltinMatrixSet::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 6, 6);
    std::array<double, 6> new_data{};
    for(size_t i = 0; i < 6; ++i)
    {
        if(args[i].type != ValueType::DOUBLE && args[i].type != ValueType::INTEGER) {diag.error("matrix_set argument " + std::to_string(i + 1) + " must be numeric", file, line); return {};}
        new_data[i] = (args[i].type == ValueType::DOUBLE) ? std::get<double>(args[i].data) : static_cast<double>(std::get<int>(args[i].data));
    }
    current_matrix.data = new_data;
    return Value(true);
}

Value BuiltinMatrixBuild::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 5, 5);
    try
    {
        double sx = get_number(diag, file, line, args[0], "matrix_build", 1);
        double sy = get_number(diag, file, line, args[1], "matrix_build", 2);
        double rot = get_number(diag, file, line, args[2], "matrix_build", 3);
        double tx = get_number(diag, file, line, args[3], "matrix_build", 4);
        double ty = get_number(diag, file, line, args[4], "matrix_build", 5);

        current_matrix = Matrix2D::build(sx, sy, rot, tx, ty);

        std::vector<Value> result;
        for(double d : current_matrix.data)
        {
            result.emplace_back(d);
        }
        return Value(result);
    }
    catch(const std::exception& e) {diag.error(std::string("matrix_build failed: ") + e.what(), file, line); return {};}
}

Value BuiltinMatrixIdentity::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 0, 0);
    current_matrix = Matrix2D::identity();
    std::vector<Value> result;
    for(double d : current_matrix.data)
    {
        result.emplace_back(d);
    }
    return Value(result);
}

Value BuiltinMatrixMultiply::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 6, 6);
    try
    {
        Matrix2D other {{
                                get_number(diag, file, line, args[0], "matrix_multiply", 1),
                                get_number(diag, file, line, args[1], "matrix_multiply", 2),
                                get_number(diag, file, line, args[2], "matrix_multiply", 3),
                                get_number(diag, file, line, args[3], "matrix_multiply", 4),
                                get_number(diag, file, line, args[4], "matrix_multiply", 5),
                                get_number(diag, file, line, args[5], "matrix_multiply", 6)
                        }};

        current_matrix = current_matrix.multiply(other);

        std::vector<Value> result;
        for(double d : current_matrix.data)
        {
            result.emplace_back(d);
        }
        return Value(result);
    }
    catch(const std::exception& e) {diag.error(std::string("matrix_multiply failed: ") + e.what(), file, line); return {};}
}

Value BuiltinMatrixInverse::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 0, 0);
    try {current_matrix = current_matrix.inverse();}
    catch(const std::exception& e) {diag.error(std::string("matrix_inverse failed: ") + e.what(), file, line); return {};}

    std::vector<Value> result;
    for(double d : current_matrix.data)
    {
        result.emplace_back(d);
    }
    return Value(result);
}

Value BuiltinMatrixTransformVertex::invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line)
{
    ensure_arity(args, 2, 2);
    try
    {
        double x = get_number(diag, file, line, args[0], "matrix_transform_vertex", 1);
        double y = get_number(diag, file, line, args[1], "matrix_transform_vertex", 2);
        auto [tx, ty] = current_matrix.transform_vertex(x, y);
        return Value(std::vector<Value>{Value(tx), Value(ty)});
    }
    catch(const std::exception& e) {diag.error(std::string("matrix_transform_vertex failed: ") + e.what(), file, line); return {};}
}

void register_builtin_matrix_core()
{
    auto& reg = BuiltinRegistry::instance();
    reg.register_builtin(std::make_unique<BuiltinMatrixGet>());
    reg.register_builtin(std::make_unique<BuiltinMatrixSet>());
    reg.register_builtin(std::make_unique<BuiltinMatrixBuild>());
    reg.register_builtin(std::make_unique<BuiltinMatrixIdentity>());
    reg.register_builtin(std::make_unique<BuiltinMatrixMultiply>());
    reg.register_builtin(std::make_unique<BuiltinMatrixInverse>());
    reg.register_builtin(std::make_unique<BuiltinMatrixTransformVertex>());
}
