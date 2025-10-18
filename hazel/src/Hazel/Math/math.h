#pragma once

#include <glm/glm.hpp>

namespace Hazel
{
namespace Math
{

// 不会处理透视类似的东西
HAZEL_API bool decomposeTransform(const glm::mat4& transform,
                                  glm::vec3& translation,
                                  glm::vec3& rotation,
                                  glm::vec3& scale);

}  // namespace Math
}  // namespace Hazel