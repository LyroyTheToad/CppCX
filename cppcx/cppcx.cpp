#include <chrono>
#include <thread>

#include <boost/process.hpp>

#include "cppcx_conf.hpp"
#include "cppcx/cppcx.hpp"


//
// Constructor and deconstructor
//

cx::Future::Future() noexcept {}

cx::Future::~Future()
{
    if (mpKeepRunning) *mpKeepRunning = false;

    // There is no need to call `mFuture.wait()` since its deconstructor already does it
}


//
// Functions
//

cx::Result cx::Execute(const std::string& command, const uint16_t timeout)
{
    return cx::Execute(command, {}, timeout);
}

cx::Result cx::Execute(const std::string& command, const std::vector<std::string>& stdIn /*= {}*/, const uint16_t timeout /*= 10*/)
{
    cx::Result cxResult;

    if (command.empty() || command.find_first_not_of(' ') == std::string::npos)
    {
        cxResult.stdErr = "Empty command";
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
        cxResult.stdErr = "Invalid command";
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
    if (!cxResult.stdOut.empty()) cxResult.stdOut.pop_back();
    
    while (errStream && std::getline(errStream, line))
    {
        cxResult.stdErr += line + "\n";
    }
    if (!cxResult.stdErr.empty()) cxResult.stdErr.pop_back();

    if (!cxResult.stdErr.empty() && cxResult.stdOut.empty()) cxResult.success = false;

    return cxResult;
}


cx::Future cx::AsyncExecute(const std::string &command, const std::vector<std::string>& stdIn /*= {}*/)
{
    cx::Future cxFuture;

    if (command.empty() || command.find_first_not_of(' ') == std::string::npos)
    {
        cx::Result cxResult;
        cxResult.stdErr = "Empty command";
        cxResult.success = false;

        // A workaround to create a ready future
        cxFuture.mFuture = std::async(std::launch::deferred, [&cxResult](){ return cxResult; });
        cxFuture.mFuture.wait();

        return cxFuture;
    }

    const std::shared_ptr<const bool> pKeepRunning = cxFuture.mpKeepRunning;

    cxFuture.mFuture = std::async(std::launch::async, [command, pKeepRunning, stdIn]()
    {
        namespace bp = boost::process;
        using namespace std::chrono_literals;

        bp::group processGroup;
        bp::child childProcess;

        bp::ipstream outStream, errStream;
        bp::opstream inStream;
        std::string line;

        cx::Result cxResult;

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
            cxResult.stdErr = "Invalid command";
            cxResult.success = false;
            return cxResult;
        }


        for (const std::string& s : stdIn)
        {
            inStream << s << std::endl;
        }


        while(childProcess.running())
        {
            if (!(*pKeepRunning))
            {    
                cxResult.success = false;
                cxResult.timedOut = true;
                break;
            }

            std::this_thread::sleep_for(500ms);
        }

        processGroup.terminate(ec);


        while (outStream && std::getline(outStream, line))
        {
            cxResult.stdOut += line + "\n";
        }
        if (!cxResult.stdOut.empty()) cxResult.stdOut.pop_back();
        
        while (errStream && std::getline(errStream, line))
        {
            cxResult.stdErr += line + "\n";
        }
        if (!cxResult.stdErr.empty()) cxResult.stdErr.pop_back();

        if (!cxResult.stdErr.empty() && cxResult.stdOut.empty()) cxResult.success = false;

        return cxResult;

    });

    return cxFuture;
}
