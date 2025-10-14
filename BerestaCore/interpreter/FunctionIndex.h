//
// Created by Denis on 05.10.2025.
//

#ifndef BERESTALANGUAGE_FUNCTIONINDEX_H
#define BERESTALANGUAGE_FUNCTIONINDEX_H

#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

class Statement;
class FunctionStatement;
enum class StatementType;
enum class FunctionVisibility;

struct FunctionRef
{
    FunctionStatement* func {nullptr};
    std::string file;
    bool is_public {false};
};

class FunctionIndex
{
    public:
        void reindex_file(const std::string& file, const std::vector<std::unique_ptr<Statement>>& ast);

        FunctionRef* find_public(const std::string& name);
        FunctionStatement* find_private_in_file(const std::string& file, const std::string& name);

        const FunctionRef* find_function(const std::string& name, const std::string& current_file);

    private:
       std::unordered_map<std::string, FunctionRef> _public;
       std::unordered_map<std::string, std::unordered_map<std::string, FunctionStatement*>> _private_by_file;
       std::unordered_map<std::string, std::unordered_map<std::string, FunctionRef>> _private_cache;
};


#endif //BERESTALANGUAGE_FUNCTIONINDEX_H
