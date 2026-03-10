#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

namespace timer {

    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;

    inline TimePoint now() {
        return Clock::now();
    }

    inline double seconds(const TimePoint& start, const TimePoint& end) {
        return std::chrono::duration<double>(end - start).count();
    }

    inline void print(const std::string& label, double secs) {
        std::cout << std::left << std::setw(38) << label
        << ": " << std::fixed << std::setprecision(6)
        << secs << " s\n";
    }

}
