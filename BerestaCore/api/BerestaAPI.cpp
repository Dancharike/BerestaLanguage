//
// Created by Denis on 28.10.2025
//

#include "../interpreter/Interpreter.h"
#include <sstream>
#include <string>

extern "C" __declspec(dllexport)
const char* run_script(const char* code)
{
    static std::string result;

    try
    {
        Diagnostics diag;
        Environment env(&diag);
        FunctionIndex index;
        Interpreter interpreter(env, index, diag);

        std::string virtual_filename = "api_entry.beresta";
        interpreter.register_file(virtual_filename, code);

        interpreter.run_project(virtual_filename);

        std::ostringstream json;

        if(diag.has_error())
        {
            std::ostringstream err_stream;
            diag.print_all();
            json << "{" << R"("stdout":"",)" << R"("stderr":"Execution failed with diagnostics",)" << "\"success\":false" << "}";
        }
        else
        {
            json << "{" << R"("stdout":"Executed successfully.",)" << R"("stderr":"",)" << "\"success\":true" << "}";
        }

        result = json.str();
        return result.c_str();
    }
    catch(const std::exception& e)
    {
        result = std::string(R"({"stdout":"","stderr":")") + e.what() + R"(","success":false})";
        return result.c_str();
    }
}
