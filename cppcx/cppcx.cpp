#include <chrono>
#include <thread>

#include <boost/process.hpp>

#include "cppcx_conf.hpp"
#include "cppcx/cppcx.hpp"


//
// Deconstructor
//

cx::Future::~Future()
{
    if (mKeepRunning) *mKeepRunning = false;
    try { mFuture.wait(); }
    catch(const std::exception&) {}
}

cx::Result cx::Execute(const std::string& command, uint16_t timeout /*= 30*/)
{
    namespace bp = boost::process;
    
    bp::group processGroup;
    bp::child childProcess;
    bp::ipstream outStream, errorStream;

    cx::Result result;


    childProcess = bp::child(
        command,
        processGroup,
        bp::std_out > outStream,
        bp::std_err > errorStream
    );


    if (!processGroup.wait_for(std::chrono::seconds(timeout)))
    {
        processGroup.terminate();
        result.success = false;
        result.timedOut = true;
    }

    std::string line;

    while (outStream && std::getline(outStream, line))
    {
        result.stdOut += line + "\n";
    }
    if (!result.stdOut.empty()) result.stdOut.pop_back();

    while (errorStream && std::getline(errorStream, line))
    {
        result.stdErr += line + "\n";
    }
    if (!result.stdErr.empty())
    {
        result.stdErr.pop_back();
        result.success = false;
    }

    return result;
}


cx::Future cx::AsyncExecute(const std::string& command)
{
    cx::Future handler;

    handler.mFuture = std::async(std::launch::async, [&](){
        
        namespace bp = boost::process;
        using namespace std::chrono_literals;

        bp::group processGroup;
        bp::child childProcess;
        bp::ipstream outStream, errorStream;

        cx::Result result;


        childProcess = bp::child(
            command,
            processGroup,
            bp::std_out > outStream,
            bp::std_err > errorStream
        );


        while (childProcess.running())
        {
            if (!handler.mKeepRunning)
            {
                result.success = false;
                result.timedOut = true;
                break;
            }

            std::this_thread::sleep_for(500ms);
        }

        std::error_code ec;
        processGroup.terminate(ec);


        std::string line;

        while (outStream && std::getline(outStream, line))
        {
            result.stdOut += line + "\n";
        }
        if (!result.stdOut.empty()) result.stdOut.pop_back();

        while (errorStream && std::getline(errorStream, line))
        {
            result.stdErr += line + "\n";
        }
        if (!result.stdErr.empty())
        {
            result.stdErr.pop_back();
            result.success = false;
        }

        return result;
    });

    return handler;
}
