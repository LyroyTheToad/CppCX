#include <atomic>
#include <future>
#include <string>
#include <vector>

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


    class Future {

    public:

        //
        // Constructors, assignment operators and deconstructor
        //

        Future() noexcept;

        Future(const Future&) = delete;
        Future& operator=(const Future&) = delete;

        Future(Future&&) noexcept = default;
        Future& operator=(Future&&) noexcept = default;

        ~Future();


        //
        // Public functions
        //

        inline void GiveUp() { if (mpKeepRunning) *mpKeepRunning = false; }
        template<class Rep, class Period>
        inline void GiveUpIn(const std::chrono::duration<Rep, Period>& t)
        { WaitFor(t); GiveUp(); }


        //
        // Wrappers for `std::future` methods
        //

        inline Result Get() { _CheckIfValid(); return mFuture.get(); }
        inline bool Valid() const { return mFuture.valid(); }
        inline void Wait() const { _CheckIfValid(); return mFuture.wait(); }
        template<class Rep, class Period>
        inline std::future_status WaitFor(const std::chrono::duration<Rep, Period>& t) const
        { _CheckIfValid(); return mFuture.wait_for(t); }
        template<class Clock, class Duration>
        inline std::future_status WaitUntil(const std::chrono::time_point<Clock, Duration>& t) const
        { _CheckIfValid(); return mFuture.wait_until(t); }


    private:

        std::future<Result> mFuture;
        std::shared_ptr<std::atomic_bool> mpKeepRunning = std::make_shared<std::atomic_bool>(true);
        
        inline void _CheckIfValid() const { if (!Valid()) throw std::runtime_error("Accessing invalid cx::Future"); }

        // Friend to grant direct access to `mFuture` and `mpKeepRunning`
        friend Future FutureExecute(const std::string& command, const std::vector<std::string>& stdIn);
    };

    Result Execute(const std::string& command, const uint16_t timeout);
    Result Execute(const std::string& command, const std::vector<std::string>& stdIn = {}, const uint16_t timeout = 10);

    Future FutureExecute(const std::string& command, const std::vector<std::string>& stdIn = {});
};
