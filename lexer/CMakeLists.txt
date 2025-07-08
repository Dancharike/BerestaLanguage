add_library(lexer Lexer.cpp)

target_include_directories(lexer PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/token
)
