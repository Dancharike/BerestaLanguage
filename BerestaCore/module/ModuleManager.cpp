//
// Created by Denis on 12.10.2025.
//

#include "ModuleManager.h"
#include <iostream>
#include <algorithm>

Module& ModuleManager::register_module(const std::string& filename, Environment& env, FunctionIndex& index)
{
    if(auto it = _modules.find(filename); it != _modules.end()) {return *it->second;}

    auto mod = std::make_unique<Module>(filename, &env, index, _diag);
    auto* ptr = mod.get();
    _modules[filename] = std::move(mod);
    return *ptr;
}

Module* ModuleManager::get_module(const std::string& filename)
{
    auto it = _modules.find(filename);
    return (it != _modules.end()) ? it->second.get() : nullptr;
}

std::vector<std::string> ModuleManager::list_filenames() const
{
    std::vector<std::string> out;
    out.reserve(_modules.size());

    for(const auto& kv : _modules)
    {
        out.push_back(kv.first);
    }

    std::sort(out.begin(), out.end());
    return out;
}
