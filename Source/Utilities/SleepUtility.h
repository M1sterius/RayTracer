#pragma once

#ifdef _WIN32
#include "Windows.h"
#endif

#include <cstdint>
#include <chrono>
#include <thread>

/*
 *
 */
inline void PreciseSleep(const uint64_t milliseconds)
{
    #ifdef _WIN32
    timeBeginPeriod(1);
    Sleep(milliseconds);
    timeEndPeriod(1);
    #else
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    #endif
}

void BusySleep(const double seconds)
{
    const auto beginTime = std::chrono::high_resolution_clock::now();

    while (true)
    {
        const std::chrono::duration<double> passed = std::chrono::high_resolution_clock::now() - beginTime;
        if (passed.count() >= seconds) break;
    }
}
