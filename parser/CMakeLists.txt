add_library(parser
        Parser.cpp
        Statement.h
        Parser.h
)

target_include_directories(parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
