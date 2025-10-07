#include <hazel.h>
#include <Hazel/Core/entry_point.h>
#include "editor_layer.h"

namespace Hazel
{

class HazelEditorApp : public Application
{
public:
    HazelEditorApp() : Application{"引擎编辑器"}
    {
        pushLayer(new EditorLayer{});
    }
    ~HazelEditorApp() {}
};

Hazel::Application* Hazel::createApplication()
{
    return new HazelEditorApp{};
}

}  // namespace Hazel
