#include "hzpch.h"
#include "application.h"

#include "Hazel/Events/application_event.h"
#include "Hazel/Events/key_event.h"
#include "Hazel/Events/mouse_event.h"
#include "Hazel/log.h"

namespace Hazel
{

Application::Application() {}
Application::~Application() {}

void Application::run()
{
    MouseMovedEvent mouse_move{1.f, 2.f};
    HZ_INFO("{}", mouse_move.toString());
    while (true) {
    }
}

}  // namespace Hazel
