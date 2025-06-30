#include <hazel.h>

class ExampleLayer : public Hazel::Layer
{
public:
    ExampleLayer() : Layer("Example") {}

    void onUpdate() override
    {
        static int index = 1;
        if (Hazel::Input::isKeyPressed(HZ_KEY_ENTER)) {
            HZ_INFO("你按下了回车!{0}", index++);
        }
    }

    void onEvent(Hazel::Event&) override
    {
        // HZ_TRACE("{0}", e.toString());
    }
};

class SandboxApp : public Hazel::Application
{
public:
    SandboxApp()
    {
        pushLayer(new ExampleLayer{});
        // 添加imgui到覆盖层
        pushOverlay(new Hazel::ImGuiLayer{});
    }
    ~SandboxApp() {}
};

Hazel::Application* Hazel::createApplication()
{
    return new SandboxApp{};
}
