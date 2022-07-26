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

    const auto pIsRunning = cxFuture.mpIsRunning;
    const auto pStopRunning = cxFuture.mpStopRunning;
    const auto pMutex = cxFuture.mpMutex;
    const auto pConditionVar = cxFuture.mpConditionVar;

    cxFuture.mFuture = std::async(std::launch::async, [command, stdIn, pIsRunning, pStopRunning, pMutex, pConditionVar]
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
            *pIsRunning = false;
            return cxResult;
        }


        for (const std::string& s : stdIn)
        {
            inStream << s << std::endl;
        }

     
        std::thread waitingThread([&childProcess, &pConditionVar, &ec]
        {
            childProcess.wait(ec);
            pConditionVar->notify_one();
        });

        std::unique_lock lk(*pMutex);
        pConditionVar->wait(lk, [&childProcess, &pStopRunning, &cxResult]
        {
            if (!childProcess.running()) return true;
            
            if (*pStopRunning)
            {
                cxResult.success = false;
                cxResult.timedOut = true;
                return true;
            }

            return false;
        });


        processGroup.terminate(ec);
        waitingThread.join();


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
