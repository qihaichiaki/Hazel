#pragma once

#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::createApplication();

int main(int, char**)
{
    WINDOWS_USE_UTF8
    Hazel::Log::init();
    HZ_CORE_WARN("日志初始化完毕!");
    HZ_INFO("你好, 世界!");
    auto app = Hazel::createApplication();
    app->run();
    delete app;
    return 0;
}

#endif