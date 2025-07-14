cmake_minimum_required(VERSION 3.16)
project(BerestaCore)

set(CMAKE_CXX_STANDARD 20)

add_library(BerestaCore STATIC
        lexer/Lexer.cpp
        lexer/Lexer.h
        token/Token.cpp
        token/Token.h
        value/Value.cpp
        value/Value.h
        parser/ExpressionParser.cpp
        parser/ExpressionParser.h
        parser/Evaluator.cpp
        parser/Evaluator.h
        parser/Parser.cpp
        parser/Parser.h
        interpreter/Interpreter.cpp
        interpreter/Interpreter.h
        parser/StatementParser.cpp
        parser/StatementParser.h
        parser/Expression.cpp
        parser/Expression.h
        parser/Statement.cpp
        parser/Statement.h
        builtin/Builtins.cpp
        builtin/Builtins.h
)

target_include_directories(BerestaCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
