#pragma once

// 导出导入宏
#ifdef HZ_PLATFORM_WINDOWS
    #ifdef HZ_BUILD_DLL
        #define HAZEL_API __declspec(dllexport)
    #else
        #define HAZEL_API __declspec(dllimport)
    #endif
#else
    #error Hazel only support Windows!
#endif

// windows 平台将控制台程序设置为utf_8编码
#ifdef _WIN32
    #include <windows.h>
    #define WINDOWS_USE_UTF8         \
        SetConsoleOutputCP(CP_UTF8); \
        SetConsoleCP(CP_UTF8);
#else
    #define WINDOWS_USE_UTF8
#endif

// 设置日志断言
#ifdef HZ_ENABLE_ASSERTS
    #define HZ_CORE_ASSERT(c, ...)                       \
        if (!(c)) {                                      \
            HZ_CORE_ERROR("断言失败: {0}", __VA_ARGS__); \
            __debugbreak();                              \
        }
    #define HZ_ASSERT(c, ...)                           \
        if (!(c)) {                                     \
            HZ_APP_ERROR("断言失败: {0}", __VA_ARGS__); \
            __debugbreak();                             \
        }
#else
    #define HZ_CORE_ASSERT(c, ...) (void)(c);
    #define HZ_ASSERT(c, ...) (void)(c);
#endif

#define BIT(X) (1 << X)

#define HZ_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)