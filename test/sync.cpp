#include <gtest/gtest.h>

#include <cppcx/cppcx.hpp>

TEST(Sync, SimpleEcho)
{
    cx::Result r;
    
    r = cx::Execute("echo hello");
    EXPECT_TRUE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "hello");
    EXPECT_EQ(r.stdErr, "");
}

TEST(Sync, EmptyCommand)
{
    const char* STD_ERR = "Empty command";

    cx::Result r;
    
    r = cx::Execute("");
    EXPECT_FALSE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "");
    EXPECT_EQ(r.stdErr, STD_ERR);
    
    r = cx::Execute(" ");
    EXPECT_FALSE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "");
    EXPECT_EQ(r.stdErr, STD_ERR);
}

TEST(Sync, InvalidCommand)
{
    const char* STD_ERR = "Invalid command";

    cx::Result r;

    for (std::string command : {"uhsbcowiu", "\n", "\t", "\r", "\f", "\v"})
    {
        r = cx::Execute(command);
        EXPECT_FALSE(r.success);
        EXPECT_FALSE(r.timedOut);
        EXPECT_EQ(r.stdOut, "");
        EXPECT_EQ(r.stdErr, STD_ERR);
    }
}

TEST(Sync, TimedOut)
{
    cx::Result r;

    r = cx::Execute("sleep 2s", 1);
    EXPECT_FALSE(r.success);
    EXPECT_TRUE(r.timedOut);
    EXPECT_EQ(r.stdErr, "");
}
