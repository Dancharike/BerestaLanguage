cmake_minimum_required(VERSION 3.16)
project(BerestaTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.4.12
)
FetchContent_MakeAvailable(doctest)

add_executable(BerestaTest
        main.cpp
        frontend/lexer/TestLexer.cpp
        frontend/parser/TestStatementParser.cpp
        frontend/parser/TestExpressionParser.cpp
        interpreter/TestInterpreter.cpp
        runtime/evaluator/TestEvaluator.cpp
)

target_link_libraries(BerestaTest
        PRIVATE
        BerestaCore
        doctest
)

add_dependencies(BerestaTest BerestaCore)

target_include_directories(BerestaTest PRIVATE ${CMAKE_SOURCE_DIR}/BerestaCore)

add_custom_command(TARGET BerestaTest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:BerestaCore>
        $<TARGET_FILE_DIR:BerestaTest>
)

enable_testing()
add_test(NAME BerestaTest COMMAND BerestaTest)
