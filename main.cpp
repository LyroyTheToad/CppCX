// This file is for testing

#include <cppcx/cppcx.hpp>
#include <iostream>
#include <string>

int main()
{
    std::string command = "c++filt";
    auto f = cx::AsyncExecute(command, {"hello", "test"});
    f.WaitFor(std::chrono::seconds(1));
    f.GiveUp();
    auto r = f.Get();
    std::cout << "$ " << command << "\n Out:\n" << r.stdOut << "\n\n Err:\n" << r.stdErr << std::boolalpha << "\nSuccess: " << r.success << "\nTimed out: " << r.timedOut << "\n";
}
