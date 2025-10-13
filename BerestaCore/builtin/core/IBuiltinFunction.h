//
// Created by Denis on 09.10.2025.
//

#ifndef BERESTALANGUAGE_IBUILTINFUNCTION_H
#define BERESTALANGUAGE_IBUILTINFUNCTION_H

#pragma once
#include <string>
#include <vector>
#include "value/Value.h"
#include "diagnostics/Diagnostics.h"

struct IBuiltinFunction
{
    virtual ~IBuiltinFunction() = default;
    [[nodiscard]] virtual std::string name() const = 0;
    virtual Value invoke(const std::vector<Value>& args, Diagnostics& diag, const std::string& filename, int line) = 0;
};


#endif //BERESTALANGUAGE_IBUILTINFUNCTION_H
