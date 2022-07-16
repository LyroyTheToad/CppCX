cmake_minimum_required(VERSION 3.23.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# Add files
#

add_library(${THIS} SHARED
    execute.cpp
    future.cpp
    future_execute.cpp
)


#
# Includes and links
#

target_include_directories(${THIS} PUBLIC
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/include
    PRIVATE ${PROJECT_SOURCE_DIR}/include/cppcx
)

target_link_directories(${THIS} PRIVATE
    pthread
)
