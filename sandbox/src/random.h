#pragma once

#include <random>

class Random
{
public:
    static void init()
    {
        // 用一个真正随机的种子初始化随机数引擎
        s_random_engine.seed(std::random_device{}());
    }

    /// @brief 返回0.0f ~ 1.0f的随机浮点数
    static float getFloat()
    {
        return (float)s_distribution(s_random_engine) / (float)std::numeric_limits<uint32_t>::max();
    }

private:
    static std::mt19937 s_random_engine;
    static std::uniform_int_distribution<std::mt19937::result_type> s_distribution;
};