cmake_minimum_required(VERSION 3.23.2)


#
# Add files
#

add_library(
    ${THIS} SHARED
    cppcx.cpp
)


#
# Includes and links
#

target_include_directories(${THIS} PUBLIC
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/include
)

target_link_directories(${THIS} PRIVATE
    pthread
)
