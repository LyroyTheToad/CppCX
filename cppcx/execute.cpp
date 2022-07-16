#include <boost/process.hpp>

#include "execute.hpp"

cx::Result cx::Execute(const std::string& command, const uint16_t timeout)
{
    return cx::Execute(command, {}, timeout);
}

cx::Result cx::Execute(const std::string& command, const std::vector<std::string>& stdIn /*= {}*/, const uint16_t timeout /*= 10*/)
{
    cx::Result cxResult;

    if (command.empty() || command.find_first_not_of(' ') == std::string::npos)
    {
        cxResult.error = "Empty command";
        cxResult.success = false;
        return cxResult;
    }

    namespace bp = boost::process;

    bp::group processGroup;
    bp::child childProcess;

    bp::ipstream outStream, errStream;
    bp::opstream inStream;
    std::string line;

    std::error_code ec;


    childProcess = bp::child(
        command,
        processGroup,
        bp::std_in < inStream,
        bp::std_out > outStream,
        bp::std_err > errStream,
        ec
    );


    if (ec)
    {
        cxResult.error = "Invalid command";
        cxResult.success = false;
        return cxResult;
    }


    for (const std::string& s : stdIn)
    {
        inStream << s << std::endl;
    }


    if (timeout == 0)
    {
        processGroup.wait();
    }
    else if (!childProcess.wait_for(std::chrono::seconds(timeout)))
    {
        processGroup.terminate();
        cxResult.success = false;
        cxResult.timedOut = true;
    }
    

    while (outStream && std::getline(outStream, line))
    {
        cxResult.stdOut += line + "\n";
    }
    
    while (errStream && std::getline(errStream, line))
    {
        cxResult.stdErr += line + "\n";
    }

    if (!cxResult.stdErr.empty() && cxResult.stdOut.empty()) cxResult.success = false;

    return cxResult;
}
