//
// Created by Denis on 09.10.2025.
//

#ifndef BERESTALANGUAGE_BUILTINMATRIXCORE_H
#define BERESTALANGUAGE_BUILTINMATRIXCORE_H

#pragma once
#include "builtin/core/IBuiltinFunction.h"

struct BuiltinMatrixGet : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "matrix_get";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinMatrixSet : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "matrix_set";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinMatrixBuild : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "matrix_build";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinMatrixIdentity : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "matrix_identity";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinMatrixMultiply : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "matrix_multiply";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinMatrixInverse : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "matrix_inverse";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinMatrixTransformVertex : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "matrix_transform_vertex";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

void register_builtin_matrix_core();


#endif //BERESTALANGUAGE_BUILTINMATRIXCORE_H
