#include <thread>
#include <string>

int main(int argc, char* argv[])
{
    if (argc > 1)
        std::this_thread::sleep_for(std::chrono::seconds(std::stoi(argv[1])));

    return 0;
}
