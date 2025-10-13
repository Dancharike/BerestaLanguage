//
// Created by Denis on 09.10.2025.
//

#ifndef BERESTALANGUAGE_BUILTINMATHCORE_H
#define BERESTALANGUAGE_BUILTINMATHCORE_H

#pragma once
#include "builtin/core/IBuiltinFunction.h"

struct BuiltinSqr : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "sqr";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

struct BuiltinSqrt : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "sqrt";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

struct BuiltinAbs : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "abs";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

struct BuiltinRound : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "round";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

struct BuiltinFloor : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "floor";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

struct BuiltinCeil : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "ceil";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

struct BuiltinFrac : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "frac";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

struct BuiltinPower : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "power";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

struct BuiltinClamp : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "clamp";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

struct BuiltinLerp : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "lerp";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

struct BuiltinMin : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "min";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

struct BuiltinMax : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "max";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

struct BuiltinMean : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "mean";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

struct BuiltinMedian : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "median";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

struct BuiltinLn : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "ln";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

struct BuiltinLog2 : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "log2";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

struct BuiltinLog10 : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "log10";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

struct BuiltinLogn : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "logn";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& file, int line) override;
};

void register_builtin_math_core();


#endif //BERESTALANGUAGE_BUILTINMATHCORE_H
