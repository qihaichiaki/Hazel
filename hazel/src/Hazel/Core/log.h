#pragma once

#include "core.h"
#include <spdlog/spdlog.h>

namespace Hazel
{

class Log
{
public:
    HAZEL_API static void init();

    HAZEL_API static std::shared_ptr<spdlog::logger>& getCoreLogger()
    {
        return s_core_logger;
    }

    HAZEL_API static std::shared_ptr<spdlog::logger>& getClientLogger()
    {
        return s_client_logger;
    }

private:
    static std::shared_ptr<spdlog::logger> s_core_logger;
    static std::shared_ptr<spdlog::logger> s_client_logger;
};
}  // namespace Hazel

#define HZ_CORE_TRACE(...) ::Hazel::Log::getCoreLogger()->trace(__VA_ARGS__)
#define HZ_CORE_INFO(...) ::Hazel::Log::getCoreLogger()->info(__VA_ARGS__)
#define HZ_CORE_WARN(...) ::Hazel::Log::getCoreLogger()->warn(__VA_ARGS__)
#define HZ_CORE_ERROR(...) ::Hazel::Log::getCoreLogger()->error(__VA_ARGS__)
#define HZ_CORE_FATAL(...) ::Hazel::Log::getCoreLogger()->critical(__VA_ARGS__)

#define HZ_TRACE(...) ::Hazel::Log::getClientLogger()->trace(__VA_ARGS__)
#define HZ_INFO(...) ::Hazel::Log::getClientLogger()->info(__VA_ARGS__)
#define HZ_WARN(...) ::Hazel::Log::getClientLogger()->warn(__VA_ARGS__)
#define HZ_ERROR(...) ::Hazel::Log::getClientLogger()->error(__VA_ARGS__)
#define HZ_FATAL(...) ::Hazel::Log::getClientLogger()->critical(__VA_ARGS__)
