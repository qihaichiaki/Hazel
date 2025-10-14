#pragma once

/**
 * 场景序列化类
 */

#include <Hazel/Scene/scene.h>

namespace Hazel
{

class SceneSerializer
{
public:
    HAZEL_API SceneSerializer(const Ref<Scene>& scene);
    HAZEL_API ~SceneSerializer() = default;

    /// @brief 将场景序列化出去
    /// @param filepath 序列化文件地址
    HAZEL_API void serialize(const std::string& filepath);

    /// @brief 从文件中反序列化入场景
    /// @param filepath 反序列化文件地址
    /// @return 是否反序列化成功, 如果不是说明文件数据损坏
    HAZEL_API bool deserialize(const std::string& filepath);

private:
    Ref<Scene> m_scene;
};

}  // namespace Hazel