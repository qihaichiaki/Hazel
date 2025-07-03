#pragma once

/**
 * shader演示类, 后续需要搬迁
 */

namespace Hazel
{

class Shader
{
public:
    Shader(const std::string& vertex_src, const std::string& fragment_src);
    ~Shader();

    void bind();
    void unBind();

private:
    uint32_t m_renderer_id;
};

}  // namespace Hazel