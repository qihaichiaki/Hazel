#pragma once

#ifdef HZ_PLATFORM_WINDOWS

extern Hazel::Application* Hazel::createApplication();

int main(int, char**)
{
    WINDOWS_USE_UTF8

    Hazel::Log::init();
    HZ_INFO("你好, 世界!");

    HZ_PROFILE_BEGIN_SESSION("Startup", "HazelProfile_Startup.json");
    auto app = Hazel::createApplication();
    HZ_PROFILE_END_SESSION();

    HZ_PROFILE_BEGIN_SESSION("Runtime", "HazelProfile_Runtime.json");
    app->run();
    HZ_PROFILE_END_SESSION();

    HZ_PROFILE_BEGIN_SESSION("Startup", "HazelProfile_Shutdown.json");
    delete app;
    HZ_PROFILE_END_SESSION();
    return 0;
}

#endif