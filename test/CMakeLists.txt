cmake_minimum_required(VERSION 3.0.0)

set(THIS_TEST cppcx_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


#
# Tests
#

set(
    CPPCX_TEST_FILES
    sync.cpp
    future.cpp
)

add_executable(${THIS_TEST} ${CPPCX_TEST_FILES})


#
# Test commands
#

set(
    TEST_COMMANDS
    print.cpp
    std_in.cpp
    wait.cpp
)

foreach(file ${TEST_COMMANDS})
    get_filename_component(name ${file} NAME_WE)
    add_executable(${name} test_commands/${file})
    list(APPEND TEST_COMMANDS_NAMES ${name})
endforeach()

add_dependencies(${THIS_TEST} ${TEST_COMMANDS_NAMES})


#
# Includes and links
#

target_link_libraries(${THIS_TEST} PRIVATE
    ${THIS}
    gtest_main
)


#
# Google Test
#

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
