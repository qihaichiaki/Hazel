#include "hzpch.h"
#include "opengl_shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>

namespace Hazel
{

static GLenum shaderTypeFromString(const std::string& type)
{
    if (type == "vertex") {
        return GL_VERTEX_SHADER;
    } else if (type == "fragment" || type == "pixel") {
        return GL_FRAGMENT_SHADER;
    }

    HZ_CORE_ASSERT(false, "未知的type类型!");
    return 0;
}

std::string OpenGLShader::readFile(const std::string& file_path)
{
    std::string source;
    std::ifstream ifs{file_path, std::ios::in | std::ios::binary};
    if (ifs) {
        ifs.seekg(0, std::ios::end);
        source.resize(ifs.tellg());
        ifs.seekg(0, std::ios::beg);
        ifs.read(&source[0], source.size());
        ifs.close();
    } else {
        HZ_CORE_ASSERT(false, "shader文件:{}读取失败!", file_path);
    }

    return source;
}

std::unordered_map<GLenum, std::string> OpenGLShader::preProcess(const std::string& source)
{
    std::unordered_map<GLenum, std::string> shader_sources;
    const char* type_token = "#type";
    size_t type_token_len = strlen(type_token);
    size_t pos = source.find(type_token, 0);
    while (pos != std::string::npos) {
        // 将#type 后的类型提取出匹配一下
        size_t eol = source.find_first_of("\r\n", pos);  // 找到下一行开始
        HZ_CORE_ASSERT(eol != std::string::npos, "shader文件格式未能符合预期");
        size_t begin = pos + type_token_len + 1;  // #type [开始]
        std::string type = source.substr(begin, eol - begin);
        GLenum key = shaderTypeFromString(type);
        size_t nex_line_pos = source.find_first_of("\r\n", eol);
        pos = source.find(type_token, nex_line_pos);
        shader_sources[key] = source.substr(
            nex_line_pos,
            pos - (nex_line_pos == std::string::npos ? source.size() - 1 : nex_line_pos));
    }

    return shader_sources;
}

void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shader_sources)
{
    GLuint program = glCreateProgram();
    m_renderer_id = 0;
    HZ_CORE_ASSERT(shader_sources.size() <= 2, "当前不支持解析超过2个shader以上的文件");
    std::array<GLuint, 2> shader_ids;
    int shader_index = 0;
    for (const auto& [shader_type, shader_source] : shader_sources) {
        GLuint shader_id = glCreateShader(shader_type);
        shader_ids[shader_index++] = shader_id;

        const GLchar* source = shader_source.c_str();
        glShaderSource(shader_id, 1, &source, 0);

        glCompileShader(shader_id);

        // 检查编译是否成功
        GLint isCompiled = 0;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            GLint maxLength = 0;
            glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(shader_id, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            for (int i = 0; i < shader_index; ++i) {
                glDeleteShader(shader_ids[i]);
            }

            HZ_CORE_ERROR("{}", infoLog.data());
            HZ_CORE_ASSERT(false, "{} shader 编译报错!", shader_type);
            return;
        }
        glAttachShader(program, shader_id);
    }

    glLinkProgram(program);
    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);
        // Don't leak shaders either.
        for (int i = 0; i < shader_index; ++i) {
            glDeleteShader(shader_ids[i]);
        }

        HZ_CORE_ERROR("{}", infoLog.data());
        HZ_CORE_ASSERT(false, "shader 程序链接报错!");
        return;
    }

    // Always detach shaders after a successful link.
    for (int i = 0; i < shader_index; ++i) {
        glDetachShader(program, shader_ids[i]);
    }
    m_renderer_id = program;
}

OpenGLShader::OpenGLShader(const std::string& name,
                           const std::string& vertex_src,
                           const std::string& fragment_src)
    : m_name{name}
{
    std::unordered_map<GLenum, std::string> shader_sources;
    shader_sources[GL_VERTEX_SHADER] = vertex_src;
    shader_sources[GL_FRAGMENT_SHADER] = fragment_src;
    compile(shader_sources);
}

OpenGLShader::OpenGLShader(const std::string& file_path)
{
    std::string source = readFile(file_path);
    auto shader_sources = preProcess(source);
    compile(shader_sources);

    // 编译成功后, 取出文件路径中的文件名
    auto last_slash = file_path.find_last_of("/\\");  // 匹配最后一个`/`或者`\`
    last_slash =
        last_slash == std::string::npos ? 0 : last_slash + 1;  // 找到/texture.shader的t下标位置
    auto last_dot = file_path.rfind('.');                      // 找到文件扩展名的位置
    auto count = last_dot == std::string::npos ? file_path.size() - last_slash
                                               : last_dot - last_slash;  // 计算文件名长度
    m_name = file_path.substr(last_slash, count);
}

OpenGLShader::~OpenGLShader()
{
    glDeleteProgram(m_renderer_id);
}

void OpenGLShader::bind() const
{
    glUseProgram(m_renderer_id);
}

void OpenGLShader::unBind() const
{
    glUseProgram(0);
}

void OpenGLShader::uploadUniformMat4(const std::string& uniform_name, const glm::mat4& matrix)
{
    // 从shader中找到对应的统一变量
    // 需要注意, 这里需要使用shader程序, 前面应该要进行绑定
    auto uniform_location = glGetUniformLocation(m_renderer_id, uniform_name.c_str());
    glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::uploadUniformFloat4(const std::string& uniform_name, const glm::vec4& vec4)
{
    auto uniform_location = glGetUniformLocation(m_renderer_id, uniform_name.c_str());
    glUniform4f(uniform_location, vec4.r, vec4.g, vec4.b, vec4.a);
}

void OpenGLShader::uploadUniformInt(const std::string& uniform_name, int value)
{
    auto uniform_location = glGetUniformLocation(m_renderer_id, uniform_name.c_str());
    glUniform1i(uniform_location, value);
}

}  // namespace Hazel