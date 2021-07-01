#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <assert.h>

using namespace std::chrono_literals;

class Timer
{
    using timep_t = decltype(std::chrono::steady_clock::now());

    timep_t _start = std::chrono::steady_clock::now();
    timep_t _end = {};

public:
    void tick() {
        _end = timep_t{};
        _start = std::chrono::steady_clock::now();
    }

    void tock() {
        _end = std::chrono::steady_clock::now();
    }

    std::chrono::milliseconds duration() const {
        // Use gsl_Expects if your project supports it.
        assert(_end != timep_t{} && "Timer must toc before reading the time");
        return std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start);
    }
};

#endif