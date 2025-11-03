//
// Created by Denis on 28.10.2025
//

#include "../interpreter/Interpreter.h"
#include "runtime/builtin/core/BuiltinRegistry.h"
#include <sstream>
#include <string>
#include <iostream>

static std::string escape_json(const std::string& s)
{
    std::string out;
    out.reserve(s.size() * 1.1);
    for(char c : s)
    {
        switch(c)
        {
            case '\"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            case '\t': out += "\\t";  break;
            default: out += c; break;
        }
    }
    return out;
}

extern "C" __declspec(dllexport)
const char* run_script(const char* code)
{
    static std::string result;

    std::ostringstream stdout_stream;
    std::ostringstream stderr_stream;

    try
    {
        Diagnostics diag;
        Environment env(&diag);
        FunctionIndex index;
        Interpreter interpreter(env, index, diag);

        set_active_environment(&env);

        env.set_output_streams(&stdout_stream, &stderr_stream);

        std::string virtual_filename = "api_entry.beresta";
        interpreter.register_file(virtual_filename, code);
        interpreter.run_project(virtual_filename);

        if(diag.has_error()) {diag.print_all(stderr_stream);}

        const bool success = !diag.has_error();

        std::ostringstream json;
        json << "{"
             << R"("stdout":")" << escape_json(stdout_stream.str()) << R"(",)"
             << R"("stderr":")" << escape_json(stderr_stream.str()) << R"(",)"
             << R"("success":)" << (success ? "true" : "false")
             << "}";

        result = json.str();
        return result.c_str();
    }
    catch(const std::exception& e)
    {
        std::ostringstream json;
        json << "{"
             << R"("stdout":"",)"
             << R"("stderr":")" << escape_json(e.what()) << R"(",)"
             << R"("success":false)"
             << "}";
        result = json.str();
        return result.c_str();
    }
}
