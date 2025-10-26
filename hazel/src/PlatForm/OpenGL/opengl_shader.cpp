#include "hzpch.h"
#include "opengl_shader.h"
#include "Hazel/Core/timer.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <filesystem>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Hazel
{

namespace Utils
{

static const char* getCacheDirectory()
{
    // TODO: make sure the assets directory is valid
    return "assets/cache/shader/opengl";
}

static void createCacheDirectoryIfNeeded()
{
    std::string cacheDirectory = getCacheDirectory();
    if (!std::filesystem::exists(cacheDirectory))
        std::filesystem::create_directories(cacheDirectory);
}

static const char* glShaderStageCachedVulkanFileExtension(uint32_t stage)
{
    switch (stage) {
        case GL_VERTEX_SHADER:
            return ".cached_vulkan.vert";
        case GL_FRAGMENT_SHADER:
            return ".cached_vulkan.frag";
    }
    HZ_CORE_ASSERT(false, "未知的shader字段类型");
    return "";
}

static const char* glShaderStageCachedOpenGLFileExtension(uint32_t stage)
{
    switch (stage) {
        case GL_VERTEX_SHADER:
            return ".cached_opengl.vert";
        case GL_FRAGMENT_SHADER:
            return ".cached_opengl.frag";
    }
    HZ_CORE_ASSERT(false, "未知的shader字段类型");
    return "";
}

static shaderc_shader_kind glShaderStageToShaderC(GLenum stage)
{
    switch (stage) {
        case GL_VERTEX_SHADER:
            return shaderc_glsl_vertex_shader;
        case GL_FRAGMENT_SHADER:
            return shaderc_glsl_fragment_shader;
    }
    HZ_CORE_ASSERT(false, "未知的shader字段类型");
    return (shaderc_shader_kind)0;
}

static const char* glShaderStageToString(GLenum stage)
{
    switch (stage) {
        case GL_VERTEX_SHADER:
            return "GL_VERTEX_SHADER";
        case GL_FRAGMENT_SHADER:
            return "GL_FRAGMENT_SHADER";
    }
    HZ_CORE_ASSERT(false, "未知的shader字段类型");
    return nullptr;
}

}  // namespace Utils

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
    HZ_PROFILE_FUNCTION();

    std::string source;
    std::ifstream ifs{file_path, std::ios::in | std::ios::binary};
    if (ifs) {
        ifs.seekg(0, std::ios::end);
        size_t size = ifs.tellg();
        if (size != -1) {
            source.resize(size);
            ifs.seekg(0, std::ios::beg);
            ifs.read(&source[0], source.size());
        } else {
            HZ_CORE_ERROR("未能从此文件夹中读取到任何的shader信息 path: '{0}'", file_path);
        }
        ifs.close();
    } else {
        HZ_CORE_ASSERT(false, "shader文件读取失败!");
    }

    return source;
}

std::unordered_map<GLenum, std::string> OpenGLShader::preProcess(const std::string& source)
{
    HZ_PROFILE_FUNCTION();

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

// void OpenGLShader::compile(const std::unordered_map<GLenum, std::string>& shader_sources)
// {
//     HZ_PROFILE_FUNCTION();

//     GLuint program = glCreateProgram();
//     m_renderer_id = 0;
//     HZ_CORE_ASSERT(shader_sources.size() <= 2 && shader_sources.size() != 0,
//                    "当前不支持解析超过2个shader或者没有shader标识的文件");
//     std::array<GLuint, 2> shader_ids;
//     int shader_index = 0;
//     for (const auto& [shader_type, shader_source] : shader_sources) {
//         GLuint shader_id = glCreateShader(shader_type);
//         shader_ids[shader_index++] = shader_id;

//         const GLchar* source = shader_source.c_str();
//         glShaderSource(shader_id, 1, &source, 0);

//         glCompileShader(shader_id);

//         // 检查编译是否成功
//         GLint isCompiled = 0;
//         glGetShaderiv(shader_id, GL_COMPILE_STATUS, &isCompiled);
//         if (isCompiled == GL_FALSE) {
//             GLint maxLength = 0;
//             glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &maxLength);

//             // The maxLength includes the NULL character
//             std::vector<GLchar> infoLog(maxLength);
//             glGetShaderInfoLog(shader_id, maxLength, &maxLength, &infoLog[0]);

//             // We don't need the shader anymore.
//             for (int i = 0; i < shader_index; ++i) {
//                 glDeleteShader(shader_ids[i]);
//             }

//             HZ_CORE_ERROR("{}", infoLog.data());
//             HZ_CORE_ASSERT(false, "{} shader 编译报错!", shader_type);
//             return;
//         }
//         glAttachShader(program, shader_id);
//     }

//     glLinkProgram(program);
//     // Note the different functions here: glGetProgram* instead of glGetShader*.
//     GLint isLinked = 0;
//     glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
//     if (isLinked == GL_FALSE) {
//         GLint maxLength = 0;
//         glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

//         // The maxLength includes the NULL character
//         std::vector<GLchar> infoLog(maxLength);
//         glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

//         // We don't need the program anymore.
//         glDeleteProgram(program);
//         // Don't leak shaders either.
//         for (int i = 0; i < shader_index; ++i) {
//             glDeleteShader(shader_ids[i]);
//         }

//         HZ_CORE_ERROR("{}", infoLog.data());
//         HZ_CORE_ASSERT(false, "shader 程序链接报错!");
//         return;
//     }

//     // Always detach shaders after a successful link.
//     for (int i = 0; i < shader_index; ++i) {
//         glDetachShader(program, shader_ids[i]);
//     }
//     m_renderer_id = program;
// }

void OpenGLShader::reflect(GLenum stage, const std::vector<uint32_t>& shader_data)
{
    spirv_cross::Compiler compiler(shader_data);
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    HZ_CORE_TRACE("OpenGLShader::Reflect - {0} {1}", Utils::glShaderStageToString(stage),
                  m_file_path.c_str());
    HZ_CORE_TRACE("    {0} uniform buffers", resources.uniform_buffers.size());
    HZ_CORE_TRACE("    {0} resources", resources.sampled_images.size());

    HZ_CORE_TRACE("Uniform buffers:");
    for (const auto& resource : resources.uniform_buffers) {
        const auto& bufferType = compiler.get_type(resource.base_type_id);
        uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
        uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
        int memberCount = bufferType.member_types.size();

        HZ_CORE_TRACE("  {0}", resource.name);
        HZ_CORE_TRACE("    Size = {0}", bufferSize);
        HZ_CORE_TRACE("    Binding = {0}", binding);
        HZ_CORE_TRACE("    Members = {0}", memberCount);
    }
}

void OpenGLShader::compileOrGetVulkanBinaries(
    const std::unordered_map<GLenum, std::string>& shader_sources)
{
    // 准备工作
    // GLSL/HLSL 代码 编译成 SPIR-V 二进制
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
    const bool optimize = true;
    if (optimize) {
        // Shaderc以 性能优先 优化 SPIR-V 代码
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
    }

    std::filesystem::path cache_directory = Utils::getCacheDirectory();

    auto& shader_data = m_vulkan_SPIRV;
    shader_data.clear();
    for (const auto& [stage, source] : shader_sources) {
        std::filesystem::path shader_file_path = m_file_path;
        // 得到对应shader的缓存路径
        std::filesystem::path cached_path =
            cache_directory / (shader_file_path.filename().string() +
                               Utils::glShaderStageCachedVulkanFileExtension(stage));

        std::ifstream in(cached_path, std::ios::in | std::ios::binary);
        // 缓存文件存在, 注意, 如果shader文件发生变动, 读取到缓存文件还是之前的, 并不会重新编译,
        // 后续可以检测shader文件的哈希是否变化判断是否重新生成缓存文件
        if (in.is_open()) {
            in.seekg(0, std::ios::end);
            auto size = in.tellg();
            in.seekg(0, std::ios::beg);

            auto& data = shader_data[stage];
            data.resize(size / sizeof(uint32_t));
            in.read((char*)data.data(), size);  // 将缓存文件的数据读入std::vector<uint32_t>内
        } else {
            // 开始编译
            shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
                source, Utils::glShaderStageToShaderC(stage), m_file_path.c_str(), options);
            if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
                HZ_CORE_ERROR(module.GetErrorMessage());
                HZ_CORE_ASSERT(false, "SpvCompilationResult失败!");
            }

            shader_data[stage] = std::vector<uint32_t>{module.cbegin(), module.cend()};

            // bin 缓存文件保存
            std::ofstream out(cached_path, std::ios::out | std::ios::binary);
            if (out.is_open()) {
                auto& data = shader_data[stage];
                out.write((char*)data.data(), data.size() * sizeof(uint32_t));
                out.flush();
                out.close();
            }
        }
    }

    for (const auto& [stage, data] : shader_data) {
        // 反射, 将编译结果反应出来
        reflect(stage, data);
    }
}

void OpenGLShader::compileOrGetOpenGLBinaries()
{
    auto& shader_data = m_opengl_SPIRV;

    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
    options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
    const bool optimize = false;
    if (optimize) {
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
    }

    std::filesystem::path cache_directory = Utils::getCacheDirectory();

    shader_data.clear();
    m_opengl_source_code.clear();

    for (const auto& [stage, spirv] : m_vulkan_SPIRV) {
        std::filesystem::path shader_file_path = m_file_path;
        std::filesystem::path cached_path =
            cache_directory / (shader_file_path.filename().string() +
                               Utils::glShaderStageCachedOpenGLFileExtension(stage));

        // 想找到opengl bin的缓存文件
        std::ifstream in(cached_path, std::ios::in | std::ios::binary);
        if (in.is_open()) {
            in.seekg(0, std::ios::end);
            auto size = in.tellg();
            in.seekg(0, std::ios::beg);

            auto& data = shader_data[stage];
            data.resize(size / sizeof(uint32_t));
            in.read((char*)data.data(), size);
        } else {
            // 否则, 就进行交叉编译
            // 先翻译为glsl code
            spirv_cross::CompilerGLSL glslCompiler(spirv);
            m_opengl_source_code[stage] = glslCompiler.compile();
            auto& source = m_opengl_source_code[stage];

            shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(
                source, Utils::glShaderStageToShaderC(stage), m_file_path.c_str());
            if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
                HZ_CORE_ERROR(module.GetErrorMessage());
                HZ_CORE_ASSERT(false, "SpvCompilationResult失败!");
            }

            shader_data[stage] = std::vector<uint32_t>{module.cbegin(), module.cend()};

            // opengl bin 缓存文件
            std::ofstream out(cached_path, std::ios::out | std::ios::binary);
            if (out.is_open()) {
                auto& data = shader_data[stage];
                out.write((char*)data.data(), data.size() * sizeof(uint32_t));
                out.flush();
                out.close();
            }
        }
    }
}

void OpenGLShader::createProgram()
{
    GLuint program = glCreateProgram();

    std::vector<GLuint> shader_IDs;
    for (auto&& [stage, spirv] : m_opengl_SPIRV) {
        GLuint shader_ID =
            shader_IDs.emplace_back(glCreateShader(stage));  // 先创建对应类型的shader
        glShaderBinary(1, &shader_ID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(),
                       spirv.size() * sizeof(uint32_t));             // 加载shader二进制内容
        glSpecializeShader(shader_ID, "main", 0, nullptr, nullptr);  // main类型?
        glAttachShader(program, shader_ID);                          // 附加到shader程序上
    }

    glLinkProgram(program);  // 链接shader程序

    // 检查shader程序状态
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
        for (auto id : shader_IDs) {
            glDeleteShader(id);
        }

        HZ_CORE_ERROR("{}", infoLog.data());
        HZ_CORE_ASSERT(false, "shader 程序链接报错!");
        return;
    }

    // Always detach shaders after a successful link.
    for (auto id : shader_IDs) {
        glDetachShader(program, id);
        glDeleteShader(id);
    }

    m_renderer_id = program;
}

OpenGLShader::OpenGLShader(const std::string& file_path) : m_file_path{file_path}
{
    HZ_PROFILE_FUNCTION();

    // shader的缓存目录文件夹准备好
    Utils::createCacheDirectoryIfNeeded();

    std::string source = readFile(file_path);
    auto shader_sources = preProcess(source);

    {
        // 耗时统计
        Timer timer;
        // 1. vulkan->spir-v
        compileOrGetVulkanBinaries(shader_sources);
        // 2. vulkan bin -> opengl bin
        compileOrGetOpenGLBinaries();
        // 3. create shader program
        createProgram();
        HZ_CORE_WARN("Shader path:{0} 编译链接耗时: {1} ms", file_path.c_str(),
                     timer.elapsedMillis());
    }

    // 编译成功后, 取出文件路径中的文件名
    auto last_slash = file_path.find_last_of("/\\");  // 匹配最后一个`/`或者`\`
    last_slash =
        last_slash == std::string::npos ? 0 : last_slash + 1;  // 找到/texture.shader的t下标位置
    auto last_dot = file_path.rfind('.');                      // 找到文件扩展名的位置
    auto count = last_dot == std::string::npos ? file_path.size() - last_slash
                                               : last_dot - last_slash;  // 计算文件名长度
    m_name = file_path.substr(last_slash, count);
}

OpenGLShader::OpenGLShader(const std::string& name,
                           const std::string& vertex_src,
                           const std::string& fragment_src)
    : m_name{name}
{
    HZ_PROFILE_FUNCTION();

    std::unordered_map<GLenum, std::string> shader_sources;
    shader_sources[GL_VERTEX_SHADER] = vertex_src;
    shader_sources[GL_FRAGMENT_SHADER] = fragment_src;

    {
        Timer timer;
        compileOrGetVulkanBinaries(shader_sources);
        compileOrGetOpenGLBinaries();
        createProgram();
        HZ_CORE_WARN("Shader name:{0} 编译链接耗时: {1} ms", name.c_str(), timer.elapsedMillis());
    }
}

OpenGLShader::~OpenGLShader()
{
    HZ_PROFILE_FUNCTION();

    glDeleteProgram(m_renderer_id);
}

void OpenGLShader::bind() const
{
    HZ_PROFILE_FUNCTION();

    glUseProgram(m_renderer_id);
}

void OpenGLShader::unBind() const
{
    HZ_PROFILE_FUNCTION();

    glUseProgram(0);
}

void OpenGLShader::setMat4(const std::string& name, const glm::mat4& value)
{
    HZ_PROFILE_FUNCTION();

    uploadUniformMat4(name, value);
}

void OpenGLShader::setFloat4(const std::string& name, const glm::vec4& value)
{
    HZ_PROFILE_FUNCTION();

    uploadUniformFloat4(name, value);
}

void OpenGLShader::setInt(const std::string& name, int value)
{
    HZ_PROFILE_FUNCTION();

    uploadUniformInt(name, value);
}

void OpenGLShader::setIntArray(const std::string& name, int* value, uint32_t count)
{
    HZ_PROFILE_FUNCTION();

    auto uniform_location = glGetUniformLocation(m_renderer_id, name.c_str());
    glUniform1iv(uniform_location, count, value);
}

void OpenGLShader::setFloat(const std::string& name, float value)
{
    HZ_PROFILE_FUNCTION();

    uploadUniformFloat(name, value);
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

void OpenGLShader::uploadUniformFloat(const std::string& uniform_name, float value)
{
    auto uniform_location = glGetUniformLocation(m_renderer_id, uniform_name.c_str());
    glUniform1f(uniform_location, value);
}

}  // namespace Hazel