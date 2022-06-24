// This file is for testing (will be removed in future)

#include <cppcx/cppcx.hpp>
#include <iostream>
#include <string>

int main()
{
    std::string command = "echo ciao";
    auto f = cx::AsyncExecute(command);
    f.GiveUpIn(std::chrono::seconds(1));
    auto r = f.Get();
    std::cout << "$ " << command << "\n Out:\n" << r.stdOut << "\n Err:\n" << r.stdErr << std::boolalpha << "\nSuccess: " << r.success << "\nTimed out: " << r.timedOut << "\n";
}

// TODO sistema i build_commands 
