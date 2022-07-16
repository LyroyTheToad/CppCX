#ifndef EXECUTE_HPP
#define EXECUTE_HPP


#include <string>
#include <vector>

#include "result.hpp"

namespace cx
{

Result Execute(const std::string& command, const uint16_t timeout);
Result Execute(const std::string& command, const std::vector<std::string>& stdIn = {}, const uint16_t timeout = 10);

}


#endif // EXECUTE_HPP
