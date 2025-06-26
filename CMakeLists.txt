cmake_minimum_required(VERSION 3.20.0)
project(Hazel VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_subdirectory(${CMAKE_SOURCE_DIR}/hazel)
add_subdirectory(${CMAKE_SOURCE_DIR}/sandbox)