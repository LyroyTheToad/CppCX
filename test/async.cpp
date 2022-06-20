#include <gtest/gtest.h>

#include <cppcx/cppcx.hpp>

TEST(Async, SimpleEcho)
{
    cx::Result r;
    cx::Future f;

    f = cx::AsyncExecute("echo hello");
    r = f.Get();
    EXPECT_TRUE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "hello");
    EXPECT_EQ(r.stdErr, "");
}

TEST(Async, EmptyCommand)
{
    const char* STD_ERR = "Empty command";

    cx::Result r;
    cx::Future f;
    
    f = cx::AsyncExecute("");
    r = f.Get();
    EXPECT_FALSE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "");
    EXPECT_EQ(r.stdErr, STD_ERR);
    
    f = cx::AsyncExecute(" ");
    r = f.Get();
    EXPECT_FALSE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "");
    EXPECT_EQ(r.stdErr, STD_ERR);
}

TEST(Async, InvalidCommand)
{
    const char* STD_ERR = "Invalid command";

    cx::Result r;
    cx::Future f;

    for (std::string command : {"uhsbcowiu", "\n", "\t", "\r", "\f", "\v"})
    {
        f = cx::AsyncExecute(command);
        r = f.Get();
        EXPECT_FALSE(r.success);
        EXPECT_FALSE(r.timedOut);
        EXPECT_EQ(r.stdOut, "");
        EXPECT_EQ(r.stdErr, STD_ERR);
    }
}

TEST(Async, TimedOut)
{
    cx::Result r;
    cx::Future f;

    f = cx::AsyncExecute("sleep 2s");
    f.GiveUp();
    r = f.Get();
    EXPECT_FALSE(r.success);
    EXPECT_TRUE(r.timedOut);
    EXPECT_EQ(r.stdErr, "");
}

TEST(Async, MoveBeforeGet)
{
    cx::Result r;
    cx::Future f1;
    cx::Future f2;

    f1 = cx::AsyncExecute("echo hello");
    f2 = std::move(f1);
    r = f2.Get();
    EXPECT_TRUE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "hello");
    EXPECT_EQ(r.stdErr, "");
}

TEST(Async, MoveAfterWait)
{
    cx::Result r;
    cx::Future f1;
    cx::Future f2;

    f1 = cx::AsyncExecute("echo hello");
    f1.Wait();
    f2 = std::move(f1);
    r = f2.Get();
    EXPECT_TRUE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "hello");
    EXPECT_EQ(r.stdErr, "");
}

TEST(Async, AccessingInvalidFuture)
{
    cx::Result r;
    cx::Future f1;
    cx::Future f2;

    f1 = cx::AsyncExecute("echo hello");
    f1.Get();
    try {
        f1.Get();
        ADD_FAILURE();
    }
    catch (const std::runtime_error& ex)
    {
        EXPECT_EQ(std::string(ex.what()), "Accessing invalid cx::Future");
    }
    
    f1 = cx::AsyncExecute("echo hello");
    f2 = std::move(f1);
    try {
        f1.Get();
        ADD_FAILURE();
    }
    catch (const std::runtime_error& ex)
    {
        EXPECT_EQ(std::string(ex.what()), "Accessing invalid cx::Future");
    }
    EXPECT_NO_THROW(f2.Get());
}
