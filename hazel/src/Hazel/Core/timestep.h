#pragma once

/**
 * 针对deltaTime的封装类
 * 只是为了更好的使用delta time
 */

#include "base.h"

namespace Hazel
{

class HAZEL_API Timestep
{
public:
    Timestep(float time = 0.0f) : m_time{time} {}

    /// @brief 注意默认步长为s数进行使用
    operator float() const
    {
        return m_time;
    }

    /// @brief 获得当前时间步长的s数
    float getSeconds() const
    {
        return m_time;
    }

    /// @brief 获得当前时间步长的ms数
    float getMilliseconds() const
    {
        return m_time * 1000.0f;
    }

private:
    float m_time;
};
}  // namespace Hazel