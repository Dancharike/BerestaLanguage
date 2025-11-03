//
// Created by Denis on 04.10.2025.
//

#ifndef BERESTALANGUAGE_ENVIRONMENT_H
#define BERESTALANGUAGE_ENVIRONMENT_H

#pragma once
#include "api/Export.h"
#include "runtime/value/Value.h"
#include "frontend/diagnostics/Diagnostics.h"
#include <string>
#include <unordered_map>
#include <vector>

class BERESTA_API Environment
{
    public:
        explicit Environment(Diagnostics* diag, Environment* parent = nullptr);

        void push_scope();
        void pop_scope();
        void define(const std::string& name, const Value& val);
        void define_global(const std::string& name, const Value& val);

        bool assign(const std::string& name, const Value& v, const std::string& file = "", int line = -1);
        [[nodiscard]] Value get(const std::string& name, const std::string& file = "", int line = -1) const;
        [[nodiscard]] bool exists(const std::string& name) const;

        void set_output_streams(std::ostream* out, std::ostream* err)
        {
            _out = out ? out : &std::cout;
            _err = err ? err : &std::cerr;
        }

        std::ostream& out() {return *_out;}
        std::ostream& err() {return *_err;}

    private:
        std::vector<std::unordered_map<std::string, Value>> _scopes;
        Environment* _parent;
        Diagnostics* _diag;
        std::ostream* _out = &std::cout;
        std::ostream* _err = &std::cerr;
};


#endif //BERESTALANGUAGE_ENVIRONMENT_H
