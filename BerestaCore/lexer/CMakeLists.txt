add_library(lexer Lexer.cpp)

target_include_directories(lexer PUBLIC
        .
        ../token
)
