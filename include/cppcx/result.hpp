#ifndef RESULT_HPP
#define RESULT_HPP


#include <string>

namespace cx
{

struct Result
{
    bool success = true;
    bool timedOut = false;
    std::string stdOut = "";
    std::string stdErr = "";
    std::string error = "";
};

}


#endif // RESULT_HPP
