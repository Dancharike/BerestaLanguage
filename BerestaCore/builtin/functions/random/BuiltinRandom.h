//
// Created by Denis on 09.10.2025.
//

#ifndef BERESTALANGUAGE_BUILTINRANDOM_H
#define BERESTALANGUAGE_BUILTINRANDOM_H

#pragma once
#include "builtin/core/IBuiltinFunction.h"

struct BuiltinChooseFrom : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "choose_from";}
    Value invoke(const std::vector<Value>& args) override;
};

struct BuiltinRandom : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "random";}
    Value invoke(const std::vector<Value>& args) override;
};

struct BuiltinRandomRange : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "random_range";}
    Value invoke(const std::vector<Value>& args) override;
};

struct BuiltinIntRandom : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "int_random";}
    Value invoke(const std::vector<Value>& args) override;
};

struct BuiltinIntRandomRange : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "int_random_range";}
    Value invoke(const std::vector<Value>& args) override;
};

void register_builtin_random();


#endif //BERESTALANGUAGE_BUILTINRANDOM_H
