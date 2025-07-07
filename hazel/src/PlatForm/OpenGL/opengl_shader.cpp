#include "hzpch.h"
#include "opengl_shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel
{
OpenGLShader::OpenGLShader(const std::string& vertex_src, const std::string& fragment_src)
{
    // 创建shader对象, 编译, 加入程序, 链接, 分离shader
    // Create an empty vertex shader handle
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Send the vertex shader source code to GL
    // Note that std::string's .c_str is NULL character terminated.
    const GLchar* source = vertex_src.c_str();
    glShaderSource(vertexShader, 1, &source, 0);

    // Compile the vertex shader
    glCompileShader(vertexShader);

    // 检查编译是否成功
    GLint isCompiled = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

        // We don't need the shader anymore.
        glDeleteShader(vertexShader);

        // Use the infoLog as you see fit.

        HZ_CORE_ERROR("{}", infoLog.data());
        HZ_CORE_ASSERT(false, "vertex shader 编译报错!");
        return;
    }

    // Create an empty fragment shader handle
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Send the fragment shader source code to GL
    // Note that std::string's .c_str is NULL character terminated.
    source = fragment_src.c_str();
    glShaderSource(fragmentShader, 1, &source, 0);

    // Compile the fragment shader
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

        // We don't need the shader anymore.
        glDeleteShader(fragmentShader);
        // Either of them. Don't leak shaders.
        glDeleteShader(vertexShader);

        // Use the infoLog as you see fit.
        HZ_CORE_ERROR("{}", infoLog.data());
        HZ_CORE_ASSERT(false, "fragment shader 编译报错!");
        return;
    }

    // Vertex and fragment shaders are successfully compiled.
    // Now time to link them together into a program.
    // Get a program object.
    GLuint program = glCreateProgram();
    m_renderer_id = program;

    // Attach our shaders to our program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // Link our program
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
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        HZ_CORE_ERROR("{}", infoLog.data());
        HZ_CORE_ASSERT(false, "shader 程序链接报错!");
        return;
    }

    // Always detach shaders after a successful link.
    glDetachShader(program, vertexShader);
    glDetachShader(program, fragmentShader);
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

}  // namespace Hazel