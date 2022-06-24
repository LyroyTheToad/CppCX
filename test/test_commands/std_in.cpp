#include <iostream>

int main(int argc, char* argv[])
{
    std::string input;

    while (true)
    {
        std::cin >> input;
        if (input == "q") break;
        std::cout << input << std::endl;
    }

    return 0;
}
