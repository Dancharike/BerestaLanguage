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
        builtin/core/Matrix2D.cpp
        builtin/core/Matrix2D.h
        environment/Environment.h
        interpreter/FunctionIndex.h
        diagnostics/Diagnostics.h
        diagnostics/BaseContext.h
        parser/Visitors.h
        builtin/core/IBuiltinFunction.h
        builtin/core/BuiltinRegistry.cpp
        builtin/core/BuiltinRegistry.h
        builtin/core/BuiltinUtils.h
        builtin/functions/print/BuiltinPrint.cpp
        builtin/functions/print/BuiltinPrint.h
        builtin/functions/math/BuiltinMathCore.cpp
        builtin/functions/math/BuiltinMathCore.h
        builtin/functions/math/BuiltinMathTrig.cpp
        builtin/functions/math/BuiltinMathTrig.h
        builtin/functions/random/BuiltinRandom.cpp
        builtin/functions/random/BuiltinRandom.h
        builtin/functions/matrix/BuiltinMatrixCore.cpp
        builtin/functions/matrix/BuiltinMatrixCore.h
        builtin/functions/array/BuiltinArray.cpp
        builtin/functions/array/BuiltinArray.h
        builtin/functions/dictionary/BuiltinDictionary.cpp
        builtin/functions/dictionary/BuiltinDictionary.h
)

target_include_directories(BerestaCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
