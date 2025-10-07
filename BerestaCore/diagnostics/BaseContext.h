//
// Created by Denis on 07.10.2025.
//

#ifndef BERESTALANGUAGE_BASECONTEXT_H
#define BERESTALANGUAGE_BASECONTEXT_H

#pragma once
#include "../diagnostics/Diagnostics.h"
#include <string>
#include <utility>

class BaseContext
{
    public:
        explicit BaseContext(Diagnostics& diag, std::string current_file = "") : _diag(diag), _current_file(std::move(current_file)) {}

    protected:
        Diagnostics& _diag;
        std::string _current_file;

        [[nodiscard]] const std::string& current_file() const {return _current_file;}
        void set_current_file(const std::string& file) {_current_file = file;}
};


#endif //BERESTALANGUAGE_BASECONTEXT_H
