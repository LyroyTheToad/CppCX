#include <iostream>
#include <thread>

int main(int argc, char* argv[])
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::string input;

    while (true)
    {
        std::cin >> input;
        if (input == "q") break;
        std::cout << input << std::endl;
    }

    return 0;
}
