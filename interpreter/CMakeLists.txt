add_library(interpreter
        Interpreter.cpp
        Interpreter.h
)

target_include_directories(interpreter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
