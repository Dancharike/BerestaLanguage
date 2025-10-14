//
// Created by Denis on 09.10.2025.
//

#ifndef BERESTALANGUAGE_BUILTINPRINT_H
#define BERESTALANGUAGE_BUILTINPRINT_H

#pragma once
#include "runtime/builtin/core/IBuiltinFunction.h"

struct BuiltinPrint : IBuiltinFunction
{
    [[nodiscard]] std::string name() const override {return "console_print";}
    Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) override;
};


#endif //BERESTALANGUAGE_BUILTINPRINT_H
