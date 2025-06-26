namespace Hazel
{
__declspec(dllimport) void print();
}

// windows 平台将控制台程序设置为utf_8编码
#ifdef _WIN32
    #include <windows.h>
    #define WINDOWS_USE_UTF8         \
        SetConsoleOutputCP(CP_UTF8); \
        SetConsoleCP(CP_UTF8);
#else
    #define WINDOWS_USE_UTF8
#endif

void main()
{
    WINDOWS_USE_UTF8
    Hazel::print();
}