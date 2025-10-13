//
// Created by Denis on 09.10.2025.
//

#ifndef BERESTALANGUAGE_BUILTINMATHTRIG_H
#define BERESTALANGUAGE_BUILTINMATHTRIG_H

#pragma once
#include "builtin/core/IBuiltinFunction.h"

struct BuiltinSin : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "sin";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinCos : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "cos";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinTan : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "tan";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArcsin : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "arcsin";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArccos : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "arccos";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArctan : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "arctan";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinArctan2 : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "arctan2";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};


struct BuiltinDsin : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "dsin";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinDcos : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "dcos";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinDtan : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "dtan";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinDarcsin : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "darcsin";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinDarccos : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "darccos";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinDarctan : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "darctan";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinDarctan2 : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "darctan2";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};


struct BuiltinPointDirection : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "point_direction";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinPointDistance : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "point_distance";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinLengthdirX : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "lengthdir_x";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

struct BuiltinLengthdirY : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "lengthdir_y";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};

void register_builtin_math_trig();


#endif //BERESTALANGUAGE_BUILTINMATHTRIG_H
