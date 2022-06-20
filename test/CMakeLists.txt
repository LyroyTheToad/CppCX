cmake_minimum_required(VERSION 3.0.0)

set(THIS_TEST cppcx_test)

enable_testing()

add_test(NAME ${THIS_TEST} COMMAND ${THIS_TEST})


#
# Tests
#

set(
    CPPCX_TEST_FILES
    sync.cpp
    async.cpp
)

add_executable(${THIS_TEST} ${CPPCX_TEST_FILES})


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
