#include "log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Hazel
{
std::shared_ptr<spdlog::logger> Log::s_core_logger{};
std::shared_ptr<spdlog::logger> Log::s_client_logger{};

void Log::init()
{
    // %^ ... %$ 表示级别颜色渲染的地方
    // %T: 23:55:59
    // %n logger的名字
    // %v 日志的内容
    // 更多格式请参考: https://github.com/gabime/spdlog/wiki/Custom-formatting

    spdlog::set_pattern("%^[%T] %n: %v%$");
    s_core_logger = spdlog::stdout_color_mt("HAZEL");
    s_core_logger->set_level(spdlog::level::level_enum::trace);

    s_client_logger = spdlog::stdout_color_mt("APP");
    s_client_logger->set_level(spdlog::level::level_enum::trace);
}

}  // namespace Hazel