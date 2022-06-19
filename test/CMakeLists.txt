cmake_minimum_required(VERSION 3.23.2)

enable_testing()

set(THIS_TEST ${THIS}_test)

add_test(NAME ${THIS_TEST} COMMAND ${THIS_TEST})

#
# Add tests
#

set(CPPCX_TEST_FILES
    echo.cpp
)

add_executable(${THIS_TEST} ${CPPCX_TEST_FILES})


#
# Google test
#

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0    
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

target_link_libraries(${THIS_TEST} PRIVATE
    ${THIS}
    gtest_main
)
