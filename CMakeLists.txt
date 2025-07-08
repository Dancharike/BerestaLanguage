cmake_minimum_required(VERSION 3.16)
project(BerestaLanguage)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lexer)
add_subdirectory(token)
add_subdirectory(value)
add_subdirectory(parser)
add_subdirectory(interpreter)

add_executable(BerestaLanguage main.cpp
        lexer/Lexer.cpp
        lexer/Lexer.h
        token/Token.cpp
        token/Token.h
        value/Value.cpp
        value/Value.h
        parser/Statement.h
        parser/Parser.cpp
        parser/Parser.h
        interpreter/Interpreter.cpp
        interpreter/Interpreter.h
)

target_link_libraries(BerestaLanguage
        lexer
        token
        value
        parser
        interpreter
)
