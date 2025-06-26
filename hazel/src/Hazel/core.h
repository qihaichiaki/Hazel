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
