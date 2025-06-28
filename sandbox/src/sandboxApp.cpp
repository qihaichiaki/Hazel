#include <hazel.h>

class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer() : Layer("Example") {}
    void onUpdate() override
    {
        HZ_INFO("ExampleLayer被更新...");
    }

    void onEvent(Hazel::Event& e) override
    {
        HZ_TRACE("{0}", e.toString());
    }
};

class SandboxApp : public Hazel::Application
{
public:
    SandboxApp()
    {
        pushLayer(new ExampleLayer{});
    }
    ~SandboxApp() {}
};

Hazel::Application* Hazel::createApplication()
{
    return new SandboxApp{};
}
