#include <thread>

#include <boost/process.hpp>

#include "future_execute.hpp"


cx::Future cx::FutureExecute(const std::string &command, const std::vector<std::string>& stdIn /*= {}*/)
{
    cx::Future cxFuture;

    if (command.empty() || command.find_first_not_of(' ') == std::string::npos)
    {
        cx::Result cxResult;
        cxResult.error = "Empty command";
        cxResult.success = false;

        // A workaround to create a ready future
        cxFuture.mFuture = std::async(std::launch::deferred, [&cxResult](){ return cxResult; });
        cxFuture.mFuture.wait();

        *cxFuture.mpIsRunning = false;

        return cxFuture;
    }

    const std::shared_ptr<std::atomic_bool> pIsRunning = cxFuture.mpIsRunning;
    const std::shared_ptr<const std::atomic_bool> pKeepRunning = cxFuture.mpKeepRunning;

    cxFuture.mFuture = std::async(std::launch::async, [command, pIsRunning, pKeepRunning, stdIn]()
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
            cxResult.error = "Invalid command";
            cxResult.success = false;
            return cxResult;
        }


        for (const std::string& s : stdIn)
        {
            inStream << s << std::endl;
        }


        while(childProcess.running())
        {
            std::this_thread::sleep_for(100ms);

            if (!(*pKeepRunning))
            {    
                cxResult.success = false;
                cxResult.timedOut = true;
                break;
            }
        }

        processGroup.terminate(ec);


        while (outStream && std::getline(outStream, line))
        {
            cxResult.stdOut += line + "\n";
        }
        
        while (errStream && std::getline(errStream, line))
        {
            cxResult.stdErr += line + "\n";
        }

        if (!cxResult.stdErr.empty() && cxResult.stdOut.empty()) cxResult.success = false;

        *pIsRunning = false;

        return cxResult;

    });
    
    return cxFuture;
}
