#include "future.hpp"

cx::Future::Future() noexcept {}

cx::Future::~Future()
{
    if (Valid()) GiveUp();
}

cx::Future::Future(Future&& other) noexcept :
    mFuture(std::move(other.mFuture)),
    mpIsRunning(std::move(other.mpIsRunning)),
    mpStopRunning(std::move(other.mpStopRunning)),
    mpMutex(std::move(other.mpMutex)),
    mpConditionVar(std::move(other.mpConditionVar))
{
    other.mHasMoved = true;
}

cx::Future& cx::Future::operator=(Future&& other) noexcept
{
    std::swap(mFuture, other.mFuture);
    std::swap(mpIsRunning, other.mpIsRunning);
    std::swap(mpStopRunning, other.mpStopRunning);
    std::swap(mpMutex, other.mpMutex);
    std::swap(mpConditionVar, other.mpConditionVar);

    other.mHasMoved = true;

    return *this;
}


void cx::Future::GiveUp()
{
    _ThrowIfNotValid();
    
    {
        std::scoped_lock lk(*mpMutex);
        *mpStopRunning = true;
    }
    mpConditionVar->notify_one();

    Wait();
}
