#pragma once

#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::createApplication();

int main(int, char**)
{
    WINDOWS_USE_UTF8
    printf("Hazel 引擎");
    auto app = Hazel::createApplication();
    app->run();
    delete app;
    return 0;
}

#endif