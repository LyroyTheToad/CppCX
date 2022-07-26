#include <gtest/gtest.h>

#include <cppcx/cppcx.hpp>

using namespace std::chrono_literals;


#define DEFAULT_TIMEOUT 5s
#define DEFAULT_WAIT 5


TEST(Future, Print)
{
    cx::Result r;
    cx::Future f;

    f = cx::FutureExecute("./print hello");
    f.GiveUpIn(DEFAULT_TIMEOUT);

    EXPECT_FALSE(f.Running());

    r = f.Get();
    EXPECT_TRUE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "hello\n");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, "");
}


TEST(Future, EmptyCommand)
{
    std::string ERROR = "Empty command";

    cx::Result r;
    cx::Future f;
    
    for (auto command : {"", " "})
    {
        f = cx::FutureExecute(command);
        f.GiveUpIn(DEFAULT_TIMEOUT);

        EXPECT_FALSE(f.Running());

        r = f.Get();
        EXPECT_FALSE(r.success);
        EXPECT_FALSE(r.timedOut);
        EXPECT_EQ(r.stdOut, "");
        EXPECT_EQ(r.stdErr, "");
        EXPECT_EQ(r.error, ERROR);
    }
}


TEST(Future, InvalidCommand)
{
    std::string ERROR = "Invalid command";

    cx::Result r;
    cx::Future f;

    for (auto command : {"uhsbcowiu", "\n", "\t", "\r", "\f", "\v"})
    {
        f = cx::FutureExecute(command);
        f.GiveUpIn(DEFAULT_TIMEOUT);

        EXPECT_FALSE(f.Running());

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

    f = cx::FutureExecute("./wait " + std::to_string(DEFAULT_WAIT));
    EXPECT_TRUE(f.Running());
    f.GiveUp();

    EXPECT_FALSE(f.Running());

    r = f.Get();
    EXPECT_FALSE(r.success);
    EXPECT_TRUE(r.timedOut);
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, "");
}


TEST(Future, ValidAtConstruction)
{
    cx::Future f;
    EXPECT_FALSE(f.Valid());
}


TEST(Future, ValidAfterGet)
{
    cx::Future f;

    f = cx::FutureExecute("./print hello");
    f.GiveUpIn(DEFAULT_TIMEOUT);

    EXPECT_TRUE(f.Valid());
    f.Get();
    EXPECT_FALSE(f.Valid());
}


TEST(Future, ValidAfterMove)
{
    cx::Future f1;
    cx::Future f2;

    f1 = cx::FutureExecute("./print hello");
    f1.GiveUpIn(DEFAULT_TIMEOUT);

    EXPECT_TRUE(f1.Valid());
    f2 = std::move(f1);
    EXPECT_FALSE(f1.Valid());
    EXPECT_TRUE(f2.Valid());
}


TEST(Future, AccessingInvalidOrMovedFuture)
{
    cx::Future f1;
    cx::Future f2;

    f1 = cx::FutureExecute("./print hello");
    f1.GiveUpIn(DEFAULT_TIMEOUT);

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
    f1.GiveUpIn(DEFAULT_TIMEOUT);

    f2 = std::move(f1);
    try {
        f1.Get();
        ADD_FAILURE();
    }
    catch (const std::runtime_error& ex)
    {
        EXPECT_EQ(std::string(ex.what()), "Accessing moved cx::Future");
    }
    EXPECT_NO_THROW(f2.Get());
}


TEST(Future, Move)
{
    cx::Result r;
    cx::Future f1;
    cx::Future f2;

    f1 = cx::FutureExecute("./print hello");
    f1.GiveUpIn(DEFAULT_TIMEOUT);
    
    f2 = std::move(f1);

    r = f2.Get();
    EXPECT_TRUE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "hello\n");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, "");
}


TEST(Future, StdIn)
{
    cx::Result r;
    cx::Future f;

    f = cx::FutureExecute("./std_in", {"hello", "test", "q"});
    f.GiveUpIn(DEFAULT_TIMEOUT);
    
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

    std::string command = "./late_std_in";
    std::vector<std::string> stdIn = {"hello", "test", "q"};
    
    f = cx::FutureExecute(command, stdIn);
    command.clear();
    stdIn.clear();
    f.GiveUpIn(DEFAULT_TIMEOUT);
    
    r = f.Get();
    EXPECT_TRUE(r.success);
    EXPECT_FALSE(r.timedOut);
    EXPECT_EQ(r.stdOut, "hello\ntest\n");
    EXPECT_EQ(r.stdErr, "");
    EXPECT_EQ(r.error, "");
}
