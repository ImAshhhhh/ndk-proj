cmake_minimum_required(VERSION 3.10.2)
project(mynative)

# Find libcURL (pre-installed on Android NDK)
find_package(CURL REQUIRED)

add_library(
    mynative
    SHARED
    native-lib.cpp
)

# Link against libcURL and Android log
target_link_libraries(mynative
    curl
    log
)

# Include CURL headers
target_include_directories(mynative PRIVATE ${CURL_INCLUDE_DIRS})
