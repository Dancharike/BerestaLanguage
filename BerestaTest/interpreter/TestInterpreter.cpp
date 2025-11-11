//
// Created by Denis on 10.11.2025.
//

#include "doctest/doctest.h"
#include "interpreter/Interpreter.h"
#include "frontend/diagnostics/Diagnostics.h"
#include "runtime/environment/Environment.h"
#include "interpreter/FunctionIndex.h"
#include <sstream>
#include <algorithm>

static std::string run_interpreter(const std::string& main_code, const std::string& lib_code)
{
    Diagnostics diag;
    Environment env(&diag);
    FunctionIndex index;
    Interpreter interpreter(env, index, diag);

    std::ostringstream out, err;
    env.set_output_streams(&out, &err);

    interpreter.register_file("math.beresta", lib_code);
    interpreter.register_file("exe.beresta", main_code);

    interpreter.run_project("exe.beresta");

    return out.str();
}

TEST_CASE("Interpreter executes project with multiple modules and functions")
{
    const std::string lib_code = R"(
        function add(a, b)
        {
            return a + b;
        }
    )";

    const std::string main_code = R"(
        let result = add(10, 5);
        if (result > 10)
        {
            console_print("OK");
        }
        else
        {
            console_print("FAIL");
        }

        let sum = 0;
        repeat (3)
        {
            sum = sum + 1;
        }

        for (let i = 0; i < 2; i = i + 1)
        {
            console_print("Loop " + i);
        }

        console_print("Sum=" + sum);
    )";

    auto output = run_interpreter(main_code, lib_code);
    output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
    output.erase(std::remove(output.begin(), output.end(), '\n'), output.end());
    output.erase(std::remove(output.begin(), output.end(), ' '), output.end());

    CHECK_EQ(output.find("OK"), std::string::npos);
    CHECK_EQ(output.find("Loop 0"), std::string::npos);
    CHECK_EQ(output.find("Loop 1"), std::string::npos);
    CHECK_EQ(output.find("Sum=3"), std::string::npos);
}
