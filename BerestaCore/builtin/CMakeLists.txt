add_library(builtin
        Builtins.cpp
        Builtins.h
        Matrix2D.cpp
        Matrix2D.h
)

target_include_directories(builtin PUBLIC .)