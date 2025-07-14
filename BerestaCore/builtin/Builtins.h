//
// Created by Denis on 14.07.2025.
//

#ifndef BERESTALANGUAGE_BUILTINS_H
#define BERESTALANGUAGE_BUILTINS_H

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include "../value/Value.h"

using BuiltinFunction = std::function<Value(const std::vector<Value>&)>;

extern const std::unordered_map<std::string, BuiltinFunction> builtin_functions;

#endif //BERESTALANGUAGE_BUILTINS_H
