#pragma once

#include <chrono>

namespace Hazel
{

class Timer
{
public:
    HAZEL_API Timer()
    {
        reset();
    }

    HAZEL_API ~Timer() {}

    HAZEL_API void reset()
    {
        m_Start = std::chrono::high_resolution_clock::now();
    }

    HAZEL_API float elapsed()
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
                   std::chrono::high_resolution_clock::now() - m_Start)
                   .count() *
               0.001f * 0.001f * 0.001f;  // 返回s
    }

    HAZEL_API float elapsedMillis()
    {
        return elapsed() * 1000.0f;  // ms
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
};

}  // namespace Hazel