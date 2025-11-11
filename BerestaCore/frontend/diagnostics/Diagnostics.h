//
// Created by Denis on 06.10.2025.
//

#ifndef BERESTALANGUAGE_DIAGNOSTICS_H
#define BERESTALANGUAGE_DIAGNOSTICS_H

#pragma once
#include "api/Export.h"
#include <string>
#include <utility>
#include <vector>
#include <iostream>

enum class DiagnosticLevel
{
    INFO,
    WARNING,
    ERROR
};

struct Diagnostic
{
    DiagnosticLevel level;
    std::string  message;
    std::string file;
    int line = -1;

    Diagnostic(DiagnosticLevel lvl, std::string msg, std::string filename = "", int ln = -1) : level(lvl), message(std::move(msg)), file(std::move(filename)), line(ln) {}
};

class BERESTA_API Diagnostics
{
    public:
        void info(const std::string& msg, const std::string& file = "", int line = -1)
        {
            _list.emplace_back(DiagnosticLevel::INFO, msg, file, line);
        }

        void warn(const std::string& msg, const std::string& file = "", int line = -1)
        {
            _list.emplace_back(DiagnosticLevel::WARNING, msg, file, line);
        }

        void error(const std::string& msg, const std::string& file = "", int line = -1)
        {
            _list.emplace_back(DiagnosticLevel::ERROR, msg, file, line);
        }

        [[nodiscard]] bool has_error() const
        {
            for(auto& d : _list)
            {
                if(d.level == DiagnosticLevel::ERROR) {return true;}
            }
            return false;
        }

        void print_all() const {dump_to(std::cerr, true);}
        void print_all(std::ostream& os) const {dump_to(os, true);}

    private:
        std::vector<Diagnostic> _list;

        void dump_to(std::ostream& os, bool has_header) const
        {
            if(_list.empty()) {return;}

            if(has_header) {os << "\n--- DIAGNOSTICS REPORT ---\n";}

            for(const auto& d : _list)
            {
                std::string prefix;
                switch(d.level)
                {
                    case DiagnosticLevel::INFO:    {prefix = "[INFO] ";    break;}
                    case DiagnosticLevel::WARNING: {prefix = "[WARNING] "; break;}
                    case DiagnosticLevel::ERROR:   {prefix = "[ERROR] ";   break;}
                }

                os << prefix;

                if(!d.file.empty())
                {
                    os << d.file;
                    if(d.line >= 0) {os << ":" << d.line;}
                    os << " -- ";
                }

                os << d.message << "\n";
            }
        }
};


#endif //BERESTALANGUAGE_DIAGNOSTICS_H
