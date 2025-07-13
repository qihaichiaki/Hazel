#pragma once

// 提供给Hazel应用程序使用的

#include "Hazel/Core/application.h"
#include "Hazel/Core/log.h"
#include "Hazel/Core/layer.h"
#include "Hazel/ImGui/imgui_layer.h"

#include "Hazel/Events/key_event.h"
#include "Hazel/Events/mouse_event.h"

#include "Hazel/Core/timestep.h"

#include "Hazel/Core/input.h"
#include "Hazel/Core/input_codes.h"

// ==== 渲染器 ================
#include "Hazel/Renderer/renderer.h"
#include "Hazel/Renderer/renderer_command.h"
#include "Hazel/Renderer/buffer.h"
#include "Hazel/Renderer/shader.h"
#include "Hazel/Renderer/vertex_array.h"
#include "Hazel/Renderer/texture.h"
// ===========================

#include "Hazel/Renderer/ortho_graphic_camera_controller.h"