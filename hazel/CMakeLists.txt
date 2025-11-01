project(Hazel)

# 加载glfw三方依赖
# 设置构建的一些选项
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/hazel/vendor/glfw)

# 加载glad三方依赖
add_subdirectory(${CMAKE_SOURCE_DIR}/hazel/vendor/glad)

# 加载imgui三方依赖
add_subdirectory(${CMAKE_SOURCE_DIR}/hazel/vendor/imgui)

# 加载yaml-cpp
# 禁用 yaml-cpp 自身的额外组件
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "Build yaml-cpp as static" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/hazel/vendor/yaml-cpp)

# 加载vulkan SDK
if(NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "未检测到 Vulkan SDK 环境变量 VULKAN_SDK")
endif()

set(VULKAN_SDK $ENV{VULKAN_SDK})
message(STATUS "Vulkan SDK found at ${VULKAN_SDK}")
set(VULKAN_INCLUDE_DIR "${VULKAN_SDK}/Include")
set(VULKAN_LIBRARY_DIR "${VULKAN_SDK}/Lib")

# 所有 Vulkan SDK 的库
set(VULKAN_SDK_LIBS_DEBUG
    "${VULKAN_LIBRARY_DIR}/vulkan-1.lib"
    "${VULKAN_LIBRARY_DIR}/shaderc_sharedd.lib"
    "${VULKAN_LIBRARY_DIR}/spirv-cross-cored.lib"
    "${VULKAN_LIBRARY_DIR}/spirv-cross-glsld.lib"
    "${VULKAN_LIBRARY_DIR}/SPIRV-Toolsd.lib"
)

set(VULKAN_SDK_LIBS_RELEASE
    "${VULKAN_LIBRARY_DIR}/vulkan-1.lib"
    "${VULKAN_LIBRARY_DIR}/shaderc_shared.lib"
    "${VULKAN_LIBRARY_DIR}/spirv-cross-core.lib"
    "${VULKAN_LIBRARY_DIR}/spirv-cross-glsl.lib"
)

# 加载box2d
add_subdirectory(vendor/box2d)

# dll/lib 文件指明生成位置
# 跳过MSBuild自动配置子目录
set(BUILD_PATH "${CMAKE_BINARY_DIR}/bin/${BUILD_CONFIG}-${BUILD_PLATFROM}/${PROJECT_NAME}")

foreach(CONFIG IN ITEMS DEBUG RELEASE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} ${BUILD_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} ${BUILD_PATH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} ${BUILD_PATH})
endforeach()

file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image/stb_image.cpp" # 将stb_image加入引擎编译
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/ImGuizmo/ImGuizmo.cpp" # 将ImGuizmo加入引擎编译
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_precompile_headers(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/hzpch.h")

target_compile_definitions(${PROJECT_NAME} PRIVATE
    -DHZ_BUILD_DLL # 生成动态库导出信息
    -DHZ_ENABLE_ASSERTS # 启用宏断点检查
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    -D_CRT_SECURE_NO_WARNINGS # 防止sscanf类似写法警告
)

# 一些三方库的宏配置
target_compile_definitions(${PROJECT_NAME} PRIVATE
    -DGLFW_INCLUDE_NONE
    -DIMGUI_API=HAZEL_API # 让ImGuizmo参与编译的头文件同引擎导出宏定义
)

# debug宏设置
target_compile_definitions(${PROJECT_NAME} PRIVATE

    # -DHZ_PROFILE
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/hazel/vendor/spdlog/include" # 增加spdlog作为三方依赖->只需要包含头文件即可
    "${CMAKE_SOURCE_DIR}/hazel/vendor/glm" # glm 同理
    "${CMAKE_SOURCE_DIR}/hazel/vendor/stb_image"
    "${CMAKE_SOURCE_DIR}/hazel/vendor/entt/include" # ECS三方库, 也是单头文件包含
    "${CMAKE_SOURCE_DIR}/hazel/vendor/ImGuizmo" # ImGuizmo相关头文件导入
    "${VULKAN_INCLUDE_DIR}" # vulkan sdk相关include
    "${CMAKE_SOURCE_DIR}/hazel/vendor/box2d/include"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw # 三方跨平台窗口库
    glad # 三方封装现代opengl库
    opengl32 # opengl
    imgui
    yaml-cpp
    $<$<CONFIG:Debug>:${VULKAN_SDK_LIBS_DEBUG}>
    $<$<CONFIG:Release>:${VULKAN_SDK_LIBS_RELEASE}>
    box2d::box2d
)
