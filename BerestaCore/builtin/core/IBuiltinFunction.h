//
// Created by Denis on 09.10.2025.
//

#ifndef BERESTALANGUAGE_IBUILTINFUNCTION_H
#define BERESTALANGUAGE_IBUILTINFUNCTION_H

#pragma once
#include <string>
#include <vector>
#include "value/Value.h"

struct IBuiltinFunction
{
    virtual ~IBuiltinFunction() = default;
    [[nodiscard]] virtual std::string name() const = 0;
    virtual Value invoke(const std::vector<Value>& args) = 0;
};


#endif //BERESTALANGUAGE_IBUILTINFUNCTION_H
