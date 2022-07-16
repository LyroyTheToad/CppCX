#include "future.hpp"

cx::Future::Future() noexcept {}

cx::Future::~Future()
{
    if (mpKeepRunning) *mpKeepRunning = false;

    // There is no need to call `mFuture.wait()` since its deconstructor already does it
}
