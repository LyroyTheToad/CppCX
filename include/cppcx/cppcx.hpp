#include <string>
#include <future>

namespace cx
{
    struct Result
    {
        bool success = true;
        bool timedOut = false;
        std::string stdOut = "";
        std::string stdErr = "";
    };


    class Future {

    public:

        //
        // Constructors, assignment operators and deconstructor
        //

        Future() = default;

        Future(const Future&) = delete;
        Future& operator=(const Future&) = delete;

        Future(Future&& other) noexcept = default;
        Future& operator=(Future&& other) noexcept = default;

        ~Future();


        //
        // Public functions
        //

        inline void GiveUp() { if (mKeepRunning) *mKeepRunning = false; }


        //
        // Wrappers for "std::future" methods
        //

        inline Result Get() { return mFuture.get(); }
        inline bool Valid() const { return mFuture.valid(); }
        inline void Wait() const { return mFuture.wait(); }
        template<class Rep, class Period>
        inline std::future_status WaitFor(const std::chrono::duration<Rep,Period>& t) const
        { return mFuture.wait_for(t); }
        template<class Clock, class Duration>
        inline std::future_status WaitUntil(const std::chrono::time_point<Clock,Duration>& t) const
        { return mFuture.wait_until(t); }


    private:

        std::future<Result> mFuture;
        std::unique_ptr<bool> mKeepRunning = std::make_unique<bool>(true);

        // Friend to grant direct access to `mFuture` and `mKeepRunning`
        friend Future AsyncExecute(const std::string& command);
    };


    Result Execute(const std::string& command, uint16_t timeout = 30);

    Future AsyncExecute(const std::string& command);
}
