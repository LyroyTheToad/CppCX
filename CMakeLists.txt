cmake_minimum_required(VERSION 3.23.2)

set(THIS cppcx)
project(${THIS} VERSION 0.1.0 LANGUAGES CXX)

add_subdirectory(cppcx)

#
# Configuration
#

configure_file(cmake/cppcx_conf.hpp.in cppcx_conf.hpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CPPCX_BUILD_TESTS "Build an run all the tests for CPPCX" ON)


#
# Tests
#

if(CPPCX_BUILD_TESTS)
    add_subdirectory(test)
    execute_process(COMMAND ./test/test_commands/build_commands.sh WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()


# TODO Remove this
add_executable(main main.cpp)
target_link_libraries(main PRIVATE cppcx)
