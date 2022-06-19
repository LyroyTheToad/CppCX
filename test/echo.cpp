#include <gtest/gtest.h>

#include <cppcx/cppcx.hpp>


TEST(Echo, Empty)
{
    ccx::Result r = ccx::Execute("echo ciao");

    EXPECT_TRUE(r.success);
}
