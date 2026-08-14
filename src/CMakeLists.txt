cmake_minimum_required(VERSION 3.10.2)
project(mynative)

add_library(
    mynative
    SHARED
    native-lib.cpp
)

target_link_libraries(mynative log)
