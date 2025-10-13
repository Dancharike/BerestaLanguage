//
// Created by Denis on 12.10.2025.
//

#ifndef BERESTALANGUAGE_MODULEMANAGER_H
#define BERESTALANGUAGE_MODULEMANAGER_H

#pragma once
#include <unordered_map>
#include <memory>
#include "Module.h"

class ModuleManager
{
    public:
        explicit ModuleManager(Diagnostics& diag) : _diag(diag) {}

        Module& register_module(const std::string& filename, Environment& env, FunctionIndex& index);
        Module* get_module(const std::string& filename);

        std::vector<std::string> list_filenames() const;

    private:
        Diagnostics& _diag;
        std::unordered_map<std::string, std::unique_ptr<Module>> _modules;
};


#endif //BERESTALANGUAGE_MODULEMANAGER_H
