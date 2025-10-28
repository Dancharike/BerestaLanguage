cmake_minimum_required(VERSION 3.16)
project(BerestaApp)

set(CMAKE_CXX_STANDARD 20)

add_executable(BerestaApp main.cpp)

target_link_libraries(BerestaApp PRIVATE BerestaCore)
target_include_directories(BerestaApp PRIVATE ${CMAKE_SOURCE_DIR}/BerestaCore)

add_custom_command(TARGET BerestaApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:BerestaCore>
        $<TARGET_FILE_DIR:BerestaApp>)
