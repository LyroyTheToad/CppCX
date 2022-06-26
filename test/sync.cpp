#include <gtest/gtest.h>

#include <cppcx/cppcx.hpp>


TEST(Sync, SimplePrint)
{
    cx::Result r;
    
    r = cx::Execute("./print hello");
    EXPECT_TRUE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "hello\n");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, "");
}

TEST(Sync, EmptyCommand)
{
    const char* ERROR = "Empty command";

    cx::Result r;
    
    r = cx::Execute("");
    EXPECT_FALSE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, ERROR);
    
    r = cx::Execute(" ");
    EXPECT_FALSE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, ERROR);
}

TEST(Sync, InvalidCommand)
{
    const char* ERROR = "Invalid command";

    cx::Result r;

    for (std::string command : {"uhsbcowiu", "\n", "\t", "\r", "\f", "\v"})
    {
        r = cx::Execute(command);
        EXPECT_FALSE(r.success);
        EXPECT_FALSE(r.timedOut);
        EXPECT_EQ(r.stdOut, "");
        EXPECT_EQ(r.stdErr, "");
        EXPECT_EQ(r.error, ERROR);
    }
}

TEST(Sync, TimedOut)
{
    cx::Result r;

    r = cx::Execute("./wait 2", 1);
    EXPECT_FALSE(r.success);
    EXPECT_TRUE(r.timedOut);
    EXPECT_EQ(r.stdOut, "");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, "");
}

TEST(Sync, StdIn)
{
    cx::Result r;

    r = cx::Execute("./std_in", {"hello", "test", "q"}, 2);
    EXPECT_TRUE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "hello\ntest\n");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, "");
}
