project(Sandbox)

# dll/lib 文件指明生成位置
# 跳过MSBuild自动配置子目录
set(BUILD_PATH "${CMAKE_BINARY_DIR}/bin/${BUILD_CONFIG}-${BUILD_PLATFROM}/${PROJECT_NAME}")

foreach(CONFIG IN ITEMS DEBUG RELEASE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} ${BUILD_PATH})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} ${BUILD_PATH})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} ${BUILD_PATH})
endforeach()

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/hazel/src"
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/hazel/vendor/spdlog/include" # 增加spdlog作为三方依赖->只需要包含头文件即可
    "${CMAKE_SOURCE_DIR}/hazel/vendor/glm" # glm opengl math库同理
    "${CMAKE_SOURCE_DIR}/hazel/vendor/imgui"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Hazel
)