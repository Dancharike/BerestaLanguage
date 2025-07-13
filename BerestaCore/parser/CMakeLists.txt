add_library(parser
        Parser.cpp
        Parser.h
        ExpressionParser.cpp
        ExpressionParser.h
        Evaluator.cpp
        Evaluator.h
)

target_include_directories(parser PUBLIC .)
