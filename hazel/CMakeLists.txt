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
    "${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image/stb_image.cpp"
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
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/hazel/vendor/spdlog/include" # 增加spdlog作为三方依赖->只需要包含头文件即可
    "${CMAKE_SOURCE_DIR}/hazel/vendor/glm" # glm 同理
    "${CMAKE_SOURCE_DIR}/hazel/vendor/stb_image"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw # 三方跨平台窗口库
    glad # 三方封装现代opengl库
    opengl32 # opengl
    imgui
)
