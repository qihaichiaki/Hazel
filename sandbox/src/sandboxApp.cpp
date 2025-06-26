#include <hazel.h>

class SandboxApp : public Hazel::Application
{
public:
    SandboxApp() {}
    ~SandboxApp() {}
};

Hazel::Application* Hazel::createApplication()
{
    return new SandboxApp{};
}
