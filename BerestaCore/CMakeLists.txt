cmake_minimum_required(VERSION 3.16)
project(BerestaCore)

set(CMAKE_CXX_STANDARD 20)

add_library(BerestaCore STATIC
        frontend/lexer/Lexer.cpp
        frontend/lexer/Lexer.h
        frontend/token/Token.cpp
        frontend/token/Token.h
        runtime/value/Value.cpp
        runtime/value/Value.h
        frontend/parser/ExpressionParser.cpp
        frontend/parser/ExpressionParser.h
        runtime/evaluator/Evaluator.cpp
        runtime/evaluator/Evaluator.h
        frontend/parser/Parser.cpp
        frontend/parser/Parser.h
        interpreter/Interpreter.cpp
        interpreter/Interpreter.h
        frontend/parser/Expression.cpp
        frontend/parser/Expression.h
        frontend/parser/Statement.cpp
        frontend/parser/Statement.h
        runtime/builtin/core/Matrix2D.cpp
        runtime/builtin/core/Matrix2D.h
        runtime/environment/Environment.h
        interpreter/FunctionIndex.h
        interpreter/FunctionIndex.cpp
        frontend/diagnostics/Diagnostics.h
        frontend/diagnostics/BaseContext.h
        frontend/parser/Visitors.h
        runtime/builtin/core/IBuiltinFunction.h
        runtime/builtin/core/BuiltinRegistry.cpp
        runtime/builtin/core/BuiltinRegistry.h
        runtime/builtin/core/BuiltinUtils.h
        runtime/builtin/functions/print/BuiltinPrint.cpp
        runtime/builtin/functions/print/BuiltinPrint.h
        runtime/builtin/functions/math/BuiltinMathCore.cpp
        runtime/builtin/functions/math/BuiltinMathCore.h
        runtime/builtin/functions/math/BuiltinMathTrig.cpp
        runtime/builtin/functions/math/BuiltinMathTrig.h
        runtime/builtin/functions/random/BuiltinRandom.cpp
        runtime/builtin/functions/random/BuiltinRandom.h
        runtime/builtin/functions/matrix/BuiltinMatrixCore.cpp
        runtime/builtin/functions/matrix/BuiltinMatrixCore.h
        runtime/builtin/functions/array/BuiltinArray.cpp
        runtime/builtin/functions/array/BuiltinArray.h
        runtime/builtin/functions/dictionary/BuiltinDictionary.cpp
        runtime/builtin/functions/dictionary/BuiltinDictionary.h
        module/Module.cpp
        module/Module.h
        module/ModuleManager.cpp
        module/ModuleManager.h
        frontend/parser/ExpressionFactory.cpp
        frontend/parser/ExpressionFactory.h
)

target_include_directories(BerestaCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
