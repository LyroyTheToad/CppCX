#include <cppcx/cppcx.hpp>
#include <iostream>
#include <string>

int main()
{
    std::string command = "ping -c 2 www.google.com";
    auto handler = cx::AsyncExecute(command);

    handler.WaitFor(std::chrono::seconds(4));

    auto handler2(std::move(handler));

    handler2.GiveUp();
    handler.GiveUp();

    auto r = handler2.Get();
    std::cout << "$ " << command << "\nOut: " << r.stdOut << "\nErr: " << r.stdErr << "\nSuccess: " << (r.success ? "True" : "False") << "\nTimed out: " << (r.timedOut ? "True" : "False") << "\n";
}
