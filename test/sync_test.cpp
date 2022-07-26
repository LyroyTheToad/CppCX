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
    std::string ERROR = "Empty command";

    cx::Result r;
    
    for (auto command : {"", " "})
    {
        r = cx::Execute(command);

        EXPECT_FALSE(r.success);
        EXPECT_FALSE(r.timedOut);
        EXPECT_EQ(r.stdOut, "");
        EXPECT_EQ(r.stdErr, "");
        EXPECT_EQ(r.error, ERROR);
    }    
}


TEST(Sync, InvalidCommand)
{
    std::string ERROR = "Invalid command";

    cx::Result r;

    for (auto command : {"uhsbcowiu", "\n", "\t", "\r", "\f", "\v"})
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

    r = cx::Execute("./wait 5", 1);

    EXPECT_FALSE(r.success);
    EXPECT_TRUE(r.timedOut);
    EXPECT_EQ(r.stdOut, "");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, "");
}

TEST(Sync, StdIn)
{
    cx::Result r;

    r = cx::Execute("./std_in", {"hello", "test", "q"});

    EXPECT_TRUE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "hello\ntest\n");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, "");
}


TEST(Sync, ClearedParameters)
{
    cx::Result r;

    std::string command = "./late_std_in";
    std::vector<std::string> stdIn = {"hello", "test", "q"};
    
    r = cx::Execute(command, stdIn);
    command.clear();
    stdIn.clear();

    EXPECT_TRUE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "hello\ntest\n");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, "");
}
