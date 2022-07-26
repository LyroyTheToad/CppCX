#ifndef FUTURE_HPP
#define FUTURE_HPP


#include <future>
#include <vector>

#include "result.hpp"

namespace cx
{

class Future {

public:

    //
    // Constructor, deconstructor etc...
    //

    Future() noexcept;
    ~Future();

    Future(const Future&) = delete;
    Future& operator=(const Future&) = delete;

    Future(Future&& other) noexcept;
    Future& operator=(Future&& other) noexcept;


    //
    // Public functions
    //

    void GiveUp();

    template<class Rep, class Period>
    inline void GiveUpIn(const std::chrono::duration<Rep, Period>& t)
    { WaitFor(t); GiveUp(); }
    template<class Clock, class Duration>
    inline void GiveUpAt(const std::chrono::time_point<Clock, Duration>& t)
    { WaitUntil(t); GiveUp(); }

    inline bool Running() { _ThrowIfMoved(); return *mpIsRunning; }


    //
    // Wrappers for `std::future` methods
    //

    inline Result Get() { _ThrowIfNotValid(); return mFuture.get(); }
    inline bool Valid() const { return (!mHasMoved && mFuture.valid()); }
    inline void Wait() const { _ThrowIfNotValid(); return mFuture.wait(); }
    template<class Rep, class Period>
    inline std::future_status WaitFor(const std::chrono::duration<Rep, Period>& t) const
    { _ThrowIfNotValid(); return mFuture.wait_for(t); }
    template<class Clock, class Duration>
    inline std::future_status WaitUntil(const std::chrono::time_point<Clock, Duration>& t) const
    { _ThrowIfNotValid(); return mFuture.wait_until(t); }


private:
    
    inline void _ThrowIfNotValid() const { _ThrowIfMoved(); if (!mFuture.valid()) throw std::runtime_error("Accessing invalid cx::Future"); }
    inline void _ThrowIfMoved() const { if (mHasMoved) throw std::runtime_error("Accessing moved cx::Future"); }


    std::future<Result> mFuture;
    std::shared_ptr<std::atomic_bool> mpIsRunning = std::make_shared<std::atomic_bool>(true);
    std::shared_ptr<std::atomic_bool> mpStopRunning = std::make_shared<std::atomic_bool>(false);

    std::shared_ptr<std::mutex> mpMutex = std::make_shared<std::mutex>();
    std::shared_ptr<std::condition_variable> mpConditionVar = std::make_shared<std::condition_variable>();

    bool mHasMoved = false;

    // Friend to grant direct access to `mFuture` and `mpKeepRunning`
    friend Future FutureExecute(const std::string& command, const std::vector<std::string>& stdIn);
};

}


#endif // FUTURE_HPP
