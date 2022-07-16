#ifndef FUTURE_EXECUTE_HPP
#define FUTURE_EXECUTE_HPP


#include <string>
#include <vector>

#include "future.hpp"

namespace cx
{

Future FutureExecute(const std::string& command, const std::vector<std::string>& stdIn = {});

}


#endif // FUTURE_EXECUTE_HPP
