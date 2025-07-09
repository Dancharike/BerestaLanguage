add_library(parser
        Parser.cpp
        Statement.h
        Parser.h
        Expression.h
        ExpressionParser.cpp
        ExpressionParser.h
        Evaluator.cpp
        Evaluator.h
)

target_include_directories(parser PUBLIC .)
