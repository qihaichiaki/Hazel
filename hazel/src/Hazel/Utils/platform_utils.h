#pragma once

/**
 * 平台工具类
 */

namespace Hazel
{

class FileDialogs
{
public:
    /// @brief 系统打开文件框
    /// @param filter 过滤器
    /// @return 打开的文件路径
    HAZEL_API static std::string openFile(const char* filter);

    /// @brief 系统保存文件框
    /// @param filter 过滤器
    /// @return 保存的文件路径
    HAZEL_API static std::string saveFile(const char* filter);
};
};  // namespace Hazel