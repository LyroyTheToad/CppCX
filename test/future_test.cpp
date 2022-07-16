#include <gtest/gtest.h>

#include <cppcx/cppcx.hpp>

using namespace std::chrono_literals;

TEST(Future, SimplePrint)
{
    cx::Result r;
    cx::Future f;

    f = cx::FutureExecute("./print hello");
    f.GiveUpIn(100ms);
    EXPECT_FALSE(f.IsRunning());
    r = f.Get();
    EXPECT_TRUE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "hello\n");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, "");
}

TEST(Future, EmptyCommand)
{
    const char* ERROR = "Empty command";

    cx::Result r;
    cx::Future f;
    
    f = cx::FutureExecute("");
    f.GiveUpIn(100ms);
    EXPECT_FALSE(f.IsRunning());
    r = f.Get();
    EXPECT_FALSE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, ERROR);
    
    f = cx::FutureExecute(" ");
    f.GiveUpIn(100ms);
    EXPECT_FALSE(f.IsRunning());
    r = f.Get();
    EXPECT_FALSE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, ERROR);
}

TEST(Future, InvalidCommand)
{
    const char* ERROR = "Invalid command";

    cx::Result r;
    cx::Future f;

    for (std::string command : {"uhsbcowiu", "\n", "\t", "\r", "\f", "\v"})
    {
        f = cx::FutureExecute(command);
        f.GiveUpIn(100ms);
        EXPECT_FALSE(f.IsRunning());
        r = f.Get();
        EXPECT_FALSE(r.success);
        EXPECT_FALSE(r.timedOut);
        EXPECT_EQ(r.stdOut, "");
        EXPECT_EQ(r.stdErr, "");
        EXPECT_EQ(r.error, ERROR);
    }
}

TEST(Future, TimedOut)
{
    cx::Result r;
    cx::Future f;

    f = cx::FutureExecute("./wait 2");
    f.GiveUp();
    EXPECT_FALSE(f.IsRunning());
    r = f.Get();
    EXPECT_FALSE(r.success);
    EXPECT_TRUE(r.timedOut);
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, "");
}

TEST(Future, MoveBeforeGet)
{
    cx::Result r;
    cx::Future f1;
    cx::Future f2;

    f1 = cx::FutureExecute("./print hello");
    f2 = std::move(f1);
    f2.GiveUpIn(100ms);
    EXPECT_FALSE(f2.IsRunning());
    r = f2.Get();
    EXPECT_TRUE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "hello\n");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, "");
}

TEST(Future, MoveAfterWaitFor)
{
    cx::Result r;
    cx::Future f1;
    cx::Future f2;

    f1 = cx::FutureExecute("./print hello");
    f1.WaitFor(100ms);
    f2 = std::move(f1);
    EXPECT_FALSE(f2.IsRunning());
    r = f2.Get();
    EXPECT_TRUE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "hello\n");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, "");
}

TEST(Future, AccessingInvalidFuture)
{
    cx::Result r;
    cx::Future f1;
    cx::Future f2;

    f1 = cx::FutureExecute("./print hello");
    f1.GiveUpIn(100ms);
    f1.Get();
    try {
        f1.Get();
        ADD_FAILURE();
    }
    catch (const std::runtime_error& ex)
    {
        EXPECT_EQ(std::string(ex.what()), "Accessing invalid cx::Future");
    }
    
    f1 = cx::FutureExecute("./print hello");
    f2 = std::move(f1);
    f2.GiveUpIn(100ms);
    try {
        f1.Get();
        ADD_FAILURE();
    }
    catch (const std::runtime_error& ex)
    {
        EXPECT_EQ(std::string(ex.what()), "Accessing invalid cx::Future");
    }
    EXPECT_NO_THROW(f2.Get());

    f1 = cx::FutureExecute("./print hello");
    f2 = std::move(f1);
    f2.GiveUpIn(100ms);
    try {
        f1.IsRunning();
        ADD_FAILURE();
    }
    catch (const std::runtime_error& ex)
    {
        EXPECT_EQ(std::string(ex.what()), "Accessing invalid cx::Future");
    }
    EXPECT_NO_THROW(f2.IsRunning());
}

TEST(Future, StdIn)
{
    cx::Result r;
    cx::Future f;

    f = cx::FutureExecute("./std_in", {"hello", "test", "q"});
    f.GiveUpIn(100ms);
    EXPECT_FALSE(f.IsRunning());
    r = f.Get();
    EXPECT_TRUE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "hello\ntest\n");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, "");
}

TEST(Future, ClearedParameters)
{
    cx::Result r;
    cx::Future f;

    std::string command = "./std_in";
    std::vector<std::string> stdIn = {"hello", "test", "q"};
    
    f = cx::FutureExecute(command, stdIn);
    EXPECT_FALSE(f.IsRunning());
    r = f.Get();
    command.clear();
    stdIn.clear();
    EXPECT_TRUE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "hello\ntest\n");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, "");
}
